#include "BrowserState.h"
#include "FactoryPresets.h"

namespace
{
    constexpr auto kLegacyProductPrefix = "ChordBro ";

    void addPresetNameMapping (StringPairArray& inMappings, const String& inLegacyName, const String& inCurrentName)
    {
        if (inLegacyName.isNotEmpty() && inCurrentName.isNotEmpty() && inLegacyName != inCurrentName)
        {
            inMappings.set (inLegacyName, inCurrentName);
        }
    }

    bool renameManagedPresetFile (const File& inDirectory,
                                  const String& inLegacyName,
                                  const String& inCurrentName,
                                  StringPairArray& inMappings)
    {
        const File legacyFile = inDirectory.getChildFile (inLegacyName + PRESET_EXTENSION);
        const File currentFile = inDirectory.getChildFile (inCurrentName + PRESET_EXTENSION);

        addPresetNameMapping (inMappings, inLegacyName, inCurrentName);

        if (!legacyFile.existsAsFile())
        {
            return false;
        }

        if (currentFile.existsAsFile())
        {
            legacyFile.deleteFile();
            return true;
        }

        if (!System::ensureDirectoryExists (currentFile.getParentDirectory()))
        {
            return false;
        }

        return legacyFile.moveFileTo (currentFile);
    }

    bool deleteManagedPresetFile (const File& inDirectory, const String& inPresetName)
    {
        const File presetFile = inDirectory.getChildFile (inPresetName + PRESET_EXTENSION);
        return !presetFile.existsAsFile() || presetFile.deleteFile();
    }

    StringArray remapPresetNames (const StringArray& inPresetNames, const StringPairArray& inMappings)
    {
        StringArray remappedPresetNames;

        for (const auto& presetName : inPresetNames)
        {
            remappedPresetNames.addIfNotAlreadyThere (inMappings.getValue (presetName, presetName));
        }

        return remappedPresetNames;
    }
}

//==============================================================================
BrowserState::BrowserState()
{
    System::ensureDirectoryExists (PRESET_FOLDER);
    refreshData();
    mFilteredPresets = mAllPresets;
}

BrowserState::~BrowserState()
{
}

//==============================================================================
void BrowserState::refreshData()
{
    System::ensureDirectoryExists (PRESET_FOLDER);
    migrateLegacyPresetLibrary();
    FactoryPresets::ensureFactoryPresetLibraryInstalled();
    scrubFavs();
    scrubTags();

    mAllPresets.clear();
    mAllPresetFiles.clear();
    mAllPresetFiles = Presets::getSortedPresetFiles();
    mFavPresetNames = StringArray::fromTokens (mFavoritesFile.getValue ("favorites"), ";", "");

    for (int index = 0; index < mAllPresetFiles.size(); index++)
    {
        Preset preset;
        preset.indexValue = index;
        preset.fileName = mAllPresetFiles[index].getFileNameWithoutExtension();
        preset.isFavorite = mFavPresetNames.contains (preset.fileName);
        mAllPresets.add (preset);
    }
}

void BrowserState::migrateLegacyPresetLibrary()
{
    StringPairArray renamedPresetNames;
    migrateLegacyFactoryPresets (renamedPresetNames);
    migrateLegacyPremiumPresets (renamedPresetNames);

    if (renamedPresetNames.size() > 0)
    {
        migrateStoredPresetNames (renamedPresetNames);
    }
}

void BrowserState::migrateLegacyFactoryPresets (StringPairArray& renamedPresetNames)
{
    const auto mappings = FactoryPresets::getLegacyFactoryPresetNameMappings();
    const auto legacyNames = mappings.getAllKeys();

    for (const auto& legacyName : legacyNames)
    {
        const String currentName = mappings.getValue (legacyName, "");
        renameManagedPresetFile (PRESET_FOLDER, legacyName, currentName, renamedPresetNames);
    }

    for (const auto& retiredName : FactoryPresets::getRetiredFactoryPresetNames())
    {
        deleteManagedPresetFile (PRESET_FOLDER, retiredName);

        const String legacyName = retiredName == "R&B - Silk Room" ? "CB RnB - Silk Room"
                                                                     : "CB " + retiredName;
        deleteManagedPresetFile (PRESET_FOLDER, legacyName);
    }
}

void BrowserState::migrateLegacyPremiumPresets (StringPairArray& renamedPresetNames)
{
    if (!PREMIUM_PACKS_FOLDER.isDirectory())
    {
        return;
    }

    const auto premiumFiles = PREMIUM_PACKS_FOLDER.findChildFiles (File::findFiles, true, "*" + PRESET_EXTENSION);

    for (const auto& premiumFile : premiumFiles)
    {
        const String fileName = premiumFile.getFileNameWithoutExtension();

        if (fileName.startsWithIgnoreCase (kLegacyProductPrefix))
        {
            const String currentName = fileName.substring (String (kLegacyProductPrefix).length());
            renameManagedPresetFile (premiumFile.getParentDirectory(), fileName, currentName, renamedPresetNames);
        }
        else
        {
            addPresetNameMapping (renamedPresetNames, String (kLegacyProductPrefix) + fileName, fileName);
        }
    }
}

void BrowserState::migrateStoredPresetNames (const StringPairArray& renamedPresetNames)
{
    auto favoriteNames = StringArray::fromTokens (mFavoritesFile.getValue ("favorites"), ";", "");
    auto remappedFavoriteNames = remapPresetNames (favoriteNames, renamedPresetNames);

    if (favoriteNames != remappedFavoriteNames)
    {
        mFavoritesFile.setValue ("favorites", remappedFavoriteNames.joinIntoString (";"));
        mFavoritesFile.saveIfNeeded();
    }

    for (const auto& tagName : getAllTagNames())
    {
        auto taggedPresetNames = StringArray::fromTokens (mTagsFile.getValue (tagName), ";", "");
        auto remappedTaggedPresetNames = remapPresetNames (taggedPresetNames, renamedPresetNames);

        if (taggedPresetNames != remappedTaggedPresetNames)
        {
            mTagsFile.setValue (tagName, remappedTaggedPresetNames.joinIntoString (";"));
        }
    }

    mTagsFile.saveIfNeeded();
}

void BrowserState::filterPresets()
{
    mFilteredPresets.clear();

    // ALL PRESETS
    if (!mIsFavoritesOn && mSelectedTags.isEmpty() && mPresetFilterText.isEmpty())
    {
        mFilteredPresets = mAllPresets;
    }

    // ONLY FAVS
    if (mIsFavoritesOn && mSelectedTags.isEmpty() && mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (preset.isFavorite)
            {
                mFilteredPresets.add (preset);
            }
        }
    }

    // ONLY TAGS
    if (!mIsFavoritesOn && !mSelectedTags.isEmpty() && mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (isInSelectedTags (preset.indexValue))
            {
                mFilteredPresets.add (preset);
            }
        }
    }

    // ONLY SEARCH
    if (!mIsFavoritesOn && mSelectedTags.isEmpty() && !mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (preset.fileName.containsIgnoreCase (mPresetFilterText))
            {
                mFilteredPresets.add (preset);
            }
        }
    }

    // FAVS + TAGS
    if (mIsFavoritesOn && !mSelectedTags.isEmpty() && mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (preset.isFavorite && isInSelectedTags (preset.indexValue))
            {
                mFilteredPresets.add (preset);
            }
        }
    }

    // FAVS + SEARCH
    if (mIsFavoritesOn && mSelectedTags.isEmpty() && !mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (preset.isFavorite && preset.fileName.containsIgnoreCase (mPresetFilterText))
            {
                mFilteredPresets.add (preset);
            }
        }
    }

    // TAGS + SEARCH
    if (!mIsFavoritesOn && !mSelectedTags.isEmpty() && !mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (isInSelectedTags (preset.indexValue) && preset.fileName.containsIgnoreCase (mPresetFilterText))
            {
                mFilteredPresets.add (preset);
            }
        }
    }

    // FAVS + TAGS + SEARCH
    if (mIsFavoritesOn && !mSelectedTags.isEmpty() && !mPresetFilterText.isEmpty())
    {
        for (Preset& preset : mAllPresets)
        {
            if (preset.isFavorite && isInSelectedTags (preset.indexValue) && preset.fileName.containsIgnoreCase (mPresetFilterText))
            {
                mFilteredPresets.add (preset);
            }
        }
    }
}

//==============================================================================
Array<File> BrowserState::getAllPresetFiles()
{
    return mAllPresetFiles;
}

juce::Array<Preset> BrowserState::getFilteredPresets()
{
    return mFilteredPresets;
}

//==============================================================================
StringArray BrowserState::getAllTagNames()
{
    StringPairArray allTags = mTagsFile.getAllProperties();
    StringArray allTagNames = allTags.getAllKeys();
    return allTagNames;
}

bool BrowserState::isTagSelected (const String inTagName)
{
    if (isTagSelectorOn()) { return false; }
    return mSelectedTags.contains (inTagName);
}

bool BrowserState::isTagAssignable (const String inTagName)
{
    if (!isTagSelectorOn()) { return false; }
    return mAssignableTag == inTagName;
}

//==============================================================================
bool BrowserState::isInSelectedTags (const int inIndexValue)
{
    int matches = 0;
    File file = mAllPresetFiles[inIndexValue];
    if (!file.existsAsFile() || mSelectedTags.size() == 0) { return false; }

    for (int index = 0; index < mSelectedTags.size(); index++)
    {
        const String tagName = mSelectedTags[index];
        StringArray taggedPresetNames = StringArray::fromTokens (mTagsFile.getValue (tagName), ";", "");
        if (taggedPresetNames.contains (file.getFileNameWithoutExtension())) { matches += 1; }
    }

    return matches > 0;
}

bool BrowserState::isInAssignableTag (const String inPresetName)
{
    int indexValue = getUnfilteredPresetIndex (inPresetName);
    if (indexValue < 0 || indexValue >= mAllPresetFiles.size()) { return false; }
    File file = mAllPresetFiles[indexValue];
    if (!file.existsAsFile() || mAssignableTag.isEmpty()) { return false; }

    StringArray taggedPresetNames = StringArray::fromTokens (mTagsFile.getValue (mAssignableTag), ";", "");
    return taggedPresetNames.contains (file.getFileNameWithoutExtension());
}

//==============================================================================
bool BrowserState::isFavorite (const String inPresetName)
{
    int indexValue = getUnfilteredPresetIndex (inPresetName);
    if (indexValue < 0 || indexValue >= mAllPresets.size()) { return false; }
    return mAllPresets[indexValue].isFavorite;
}

int BrowserState::getUnfilteredPresetIndex (const String inPresetName)
{
    int unfilteredIndex = -1;

    for (int index = 0; index < mAllPresets.size(); index++)
    {
        String presetName = mAllPresets[index].fileName;
        if (presetName == inPresetName) { unfilteredIndex = index; }
    }

    return unfilteredIndex;
}

//==============================================================================
void BrowserState::toggleFavorites()
{
    mIsFavoritesOn = !mIsFavoritesOn;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kToggleFavorites;
    sendMessage (message, ListenerType::kSync);
}

bool BrowserState::isFavoritesOn()
{
    return mIsFavoritesOn;
}

//==============================================================================
void BrowserState::toggleTagManager()
{
    mIsTagManagerVisible = !mIsTagManagerVisible;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kToggleTagManager;
    sendMessage (message, ListenerType::kSync);
}

bool BrowserState::isTagManagerVisible()
{
    return mIsTagManagerVisible;
}

//==============================================================================
void BrowserState::toggleTagSelector()
{
    mSelectedTags.clear();
    mAssignableTag.clear();
    mIsTagSelectorOn = !mIsTagSelectorOn;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kToggleTagSelector;
    sendMessage (message, ListenerType::kSync);
}

bool BrowserState::isTagSelectorOn()
{
    return mIsTagSelectorOn;
}

//==============================================================================
void BrowserState::handleClickCreateTag()
{
    if (!System::isValidFileName (mNewTagText) || mTagsFile.containsKey (mNewTagText)) { return; }

    mTagsFile.setValue (mNewTagText, "");
    mTagsFile.saveIfNeeded();

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kTagCreated;
    sendMessage (message, ListenerType::kSync);
}

void BrowserState::handleClickDeleteTag (const String inTagName)
{
    mTagsFile.removeValue (inTagName);

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kTagDeleted;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void BrowserState::handleClickShiftTag (const String inTagName, const String inDirection)
{
    StringPairArray allTags = mTagsFile.getAllProperties();
    StringArray allTagNames = allTags.getAllKeys();

    int nextIndex;
    int count = allTagNames.size();
    int currentIndex = allTagNames.indexOf (inTagName);
    if (count == 1 || (inDirection != "UP" && inDirection != "DOWN")) { return; }

    if (inDirection == "UP") { nextIndex = (currentIndex - 1) == -1 ? (count - 1) : currentIndex - 1; }
    if (inDirection == "DOWN") { nextIndex = (currentIndex + 1) == count ? 0 : currentIndex + 1; }

    allTagNames.removeString (inTagName);
    allTagNames.insert (nextIndex, inTagName);

    mTagsFile.clear();

    for (int index = 0; index < count; index++)
    {
        const String tagName = allTagNames[index];
        const String tagValue = allTags.getValue (tagName, "");
        mTagsFile.setValue (tagName, tagValue);
    }

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kTagShifted;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void BrowserState::handleClickAssignableTag (const String inTagName)
{
    if (mAssignableTag == inTagName)
    {
        mAssignableTag.clear();
    }
    else
    {
        mAssignableTag = inTagName;
    }

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kClickAssignableTag;
    sendMessage (message, ListenerType::kSync);
}

void BrowserState::handleClickSelectableTag (const String inTagName)
{
    if (mSelectedTags.contains (inTagName))
    {
        mSelectedTags.removeString (inTagName);
    }
    else
    {
        mSelectedTags.addIfNotAlreadyThere (inTagName);
    }

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kClickSelectableTag;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void BrowserState::handleClickFavorite (const int inIndexValue)
{
    if (inIndexValue < 0 || inIndexValue >= mAllPresets.size() || inIndexValue >= mAllPresetFiles.size()) { return; }

    Preset preset = mAllPresets[inIndexValue];
    File file = mAllPresetFiles[inIndexValue];
    if (!file.existsAsFile()) { return; }

    if (preset.isFavorite)
    {
        preset.isFavorite = false;
        mFavPresetNames.removeString (file.getFileNameWithoutExtension());
    }
    else
    {
        preset.isFavorite = true;
        mFavPresetNames.addIfNotAlreadyThere (file.getFileNameWithoutExtension());
    }

    mAllPresets.set (inIndexValue, preset);
    mFavoritesFile.setValue ("favorites", mFavPresetNames.joinIntoString (";"));
    mFavoritesFile.saveIfNeeded();

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFileFavorited;
    sendMessage (message, ListenerType::kSync);
}

void BrowserState::handleClickDeletePreset (const int inIndexValue)
{
    if (inIndexValue < 0 || inIndexValue >= mAllPresetFiles.size()) { return; }

    File file = mAllPresetFiles[inIndexValue];
    if (!file.existsAsFile()) { return; }

    if (Presets::isInstalledPremiumPreset (file))
    {
        System::showMessage (AlertWindow::InfoIcon,
                             "ChordBro",
                             "Installed premium presets are managed by the ChordBro installer and cannot be deleted here.");
        return;
    }

    if (mFavPresetNames.contains (file.getFileNameWithoutExtension()))
    {
        mFavPresetNames.removeString (file.getFileNameWithoutExtension());
        mFavoritesFile.setValue ("favorites", mFavPresetNames.joinIntoString (";"));
        mFavoritesFile.saveIfNeeded();
    }

    file.deleteFile();
    refreshData();
    filterPresets();

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFileDeleted;
    sendMessage (message, ListenerType::kSync);
}

void BrowserState::handleClickPresetTagger (const int inIndexValue)
{
    if (inIndexValue < 0 || inIndexValue >= mAllPresetFiles.size()) { return; }

    File file = mAllPresetFiles[inIndexValue];
    if (!file.existsAsFile() || mAssignableTag.isEmpty()) { return; }

    StringArray taggedPresetNames = StringArray::fromTokens (mTagsFile.getValue (mAssignableTag), ";", "");

    if (taggedPresetNames.contains (file.getFileNameWithoutExtension()))
    {
        taggedPresetNames.removeString (file.getFileNameWithoutExtension());
    }
    else
    {
        taggedPresetNames.addIfNotAlreadyThere (file.getFileNameWithoutExtension());
    }

    mTagsFile.setValue (mAssignableTag, taggedPresetNames.joinIntoString (";"));
    mTagsFile.saveIfNeeded();

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kClickPresetTagger;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void BrowserState::handleNewTagTextChanged (const String inNewTagText)
{
    mNewTagText = inNewTagText;
}

void BrowserState::handlePresetFilterTextChanged (const String inPresetFilterText)
{
    mPresetFilterText = inPresetFilterText;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFilterTextChanged;
    message->messageVar1 = mPresetFilterText;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void BrowserState::handleClickLeftArrow (const String inPresetName)
{
    int index = getFilteredPresetIndex (inPresetName);

    if (index > 0) { index = index - 1; }
    else if (index == 0) { index = mFilteredPresets.size() - 1; }
    else if (index == -1 && !mFilteredPresets.isEmpty()) { index = mFilteredPresets.size() - 1; }

    int nextIndex = index >= 0 ? mFilteredPresets[index].indexValue : -1;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kCurrentIndexChanged;
    message->messageVar1 = nextIndex;
    sendMessage (message, ListenerType::kSync);
}

void BrowserState::handleClickRightArrow (const String inPresetName)
{
    int index = getFilteredPresetIndex (inPresetName);

    if (index >= 0 && index < (mFilteredPresets.size() - 1)) { index = index + 1; }
    else if (index == (mFilteredPresets.size() - 1)) { index = 0; }
    else if (index == -1 && !mFilteredPresets.isEmpty()) { index = 0; }

    int nextIndex = index >= 0 ? mFilteredPresets[index].indexValue : -1;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kCurrentIndexChanged;
    message->messageVar1 = nextIndex;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void BrowserState::scrubFavs()
{
    StringArray favPresetNames;
    StringArray favs = StringArray::fromTokens (mFavoritesFile.getValue ("favorites"), ";", "");

    for (String& fav : favs)
    {
        String favPresetName = System::getLeafName (fav);

        if (favPresetName.endsWithIgnoreCase (PRESET_EXTENSION))
        {
            favPresetName = favPresetName.dropLastCharacters (PRESET_EXTENSION.length());
        }

        const File favPresetFile = Presets::findPresetFileByName (favPresetName);
        if (favPresetFile.existsAsFile()) { favPresetNames.add (favPresetName); }
    }

    mFavoritesFile.setValue ("favorites", favPresetNames.joinIntoString (";"));
    mFavoritesFile.saveIfNeeded();
}

void BrowserState::scrubTags()
{
    StringArray tagNames = getAllTagNames();

    for (String& tagName : tagNames)
    {
        StringArray taggedPresetNames;
        StringArray tags = StringArray::fromTokens (mTagsFile.getValue (tagName), ";", "");

        for (String& taggedPresetName : tags)
        {
            const File taggedPresetFile = Presets::findPresetFileByName (taggedPresetName);
            if (taggedPresetFile.existsAsFile()) { taggedPresetNames.add (taggedPresetName); }
        }

        mTagsFile.setValue (tagName, taggedPresetNames.joinIntoString (";"));
        mTagsFile.saveIfNeeded();
    }
}

int BrowserState::getFilteredPresetIndex (const String inPresetName)
{
    int filteredIndex = -1;

    for (int index = 0; index < mFilteredPresets.size(); index++)
    {
        String presetName = mFilteredPresets[index].fileName;
        if (presetName == inPresetName) { filteredIndex = index; }
    }

    return filteredIndex;
}
