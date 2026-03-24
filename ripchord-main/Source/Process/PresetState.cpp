#include "PresetState.h"
#include "Midi.h"

//==============================================================================
PresetState::PresetState()
{
}

PresetState::~PresetState()
{
}

//==============================================================================
String PresetState::getName()
{
    return mName;
}

bool PresetState::isPresetValid()
{
    if (mName.isEmpty()) { return false; }

    juce::Array<int> chordNotes;

    for (int inputNote : getPresetInputNotes())
    {
        for (int chordNote : getChordNotes (inputNote))
        {
            chordNotes.add (chordNote);
        }
    }

    return chordNotes.size() > 0;
}

bool PresetState::isPresetModified()
{
    return mIsPresetModified;
}

void PresetState::resetEditModeInputNote()
{
    mEditModeInputNote = 0;
}

const int PresetState::getEditModeInputNote()
{
    return mEditModeInputNote;
}

juce::Array<int> PresetState::getPresetInputNotes()
{
    juce::Array<int> presetInputNotes;

    for (const auto& pair : mChords)
    {
        presetInputNotes.add (pair.first);
    }

    return presetInputNotes;
}

//==============================================================================
bool PresetState::containsChord (const int inInputNote)
{
    return mChords.count (inInputNote) > 0;
}

String PresetState::getChordName (const int inInputNote)
{
    return getChord (inInputNote).name;
}

juce::Array<int> PresetState::getChordNotes (const int inInputNote)
{
    return getChord (inInputNote).notes;
}

//==============================================================================
void PresetState::handleEditModeMouseDownOnInput (const int inInputNote)
{
    const int prevEditModeInputNote = mEditModeInputNote;
    const int nextEditModeInputNote = inInputNote == mEditModeInputNote ? 0 : inInputNote;
    bool prevEditModeInputNoteContainsChord = containsChord (prevEditModeInputNote);
    juce::Array<int> prevEditModeOutputNotes = getChordNotes (prevEditModeInputNote);
    juce::Array<int> nextEditModeOutputNotes = getChordNotes (nextEditModeInputNote);

    mEditModeInputNote = nextEditModeInputNote;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kEditModeInputNote;
    message->messageVar1 = prevEditModeInputNote;
    message->messageVar2 = nextEditModeInputNote;
    message->messageVar3 = prevEditModeInputNoteContainsChord;
    message->messageArray1 = prevEditModeOutputNotes;
    message->messageArray2 = nextEditModeOutputNotes;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleEditModeMouseDownOnOutput (const int inOutputNote)
{
    if (mEditModeInputNote == 0) { return; }

    juce::Array<int> prevEditModeOutputNotes = getChordNotes (mEditModeInputNote);
    bool shouldAddNote = !prevEditModeOutputNotes.contains (inOutputNote);

    if (shouldAddNote)
    {
        Chord presetChord = getChord (mEditModeInputNote);
        presetChord.notes.addUsingDefaultSort (inOutputNote);
        setChord (mEditModeInputNote, presetChord);
    }
    else
    {
        if (prevEditModeOutputNotes.size() > 1)
        {
            Chord presetChord = getChord (mEditModeInputNote);
            presetChord.notes.removeFirstMatchingValue (inOutputNote);
            setChord (mEditModeInputNote, presetChord);
        }
        else
        {
            mChords.erase (mEditModeInputNote);
        }
    }

    juce::Array<int> nextEditModeOutputNotes = getChordNotes (mEditModeInputNote);

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kEditModeOutputNotes;
    message->messageArray1 = prevEditModeOutputNotes;
    message->messageArray2 = nextEditModeOutputNotes;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void PresetState::handleChordNameTextChanged (String inChordName)
{
    Chord presetChord = getChord (mEditModeInputNote);
    if (mEditModeInputNote == 0 || presetChord.name == inChordName) { return; }

    presetChord.name = inChordName;
    setChord (mEditModeInputNote, presetChord);

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kChordNameTextChanged;
    message->messageVar1 = inChordName;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handlePresetNameTextChanged (String inPresetName)
{
    if (mName == inPresetName) { return; }

    if (System::isValidFileName (inPresetName))
    {
        mName = inPresetName;
        mIsPresetModified = true;
    }

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetNameTextChanged;
    message->messageVar1 = mName;
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void PresetState::handleClickCut (const int inInputNote)
{
    mClipboardChord = getChord (inInputNote);
    mChords.erase (inInputNote);
    mEditModeInputNote = 0;

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetModified;
    message->messageArray1 = getPresetInputNotes();
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickCopy (const int inInputNote)
{
    mClipboardChord = getChord (inInputNote);
}

void PresetState::handleClickPaste (const int inInputNote)
{
    if (mClipboardChord.notes.isEmpty()) { return; }

    setChord (inInputNote, mClipboardChord);
    mEditModeInputNote = 0;

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetModified;
    message->messageArray1 = getPresetInputNotes();
    sendMessage (message, ListenerType::kSync);
}

//==============================================================================
void PresetState::handleClickNew()
{
    resetPresetState (false);
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFileNew;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickSave()
{
    if (!isPresetValid() || !mIsPresetModified) { return; }

    if (!System::ensureDirectoryExists (PRESET_FOLDER))
    {
        System::showMessage (AlertWindow::WarningIcon, "ChordBro", "ChordBro could not create its preset folder.");
        return;
    }

    File prevPresetFile = PRESET_FOLDER.getChildFile (mPresetFileName);
    if (prevPresetFile.existsAsFile()) { prevPresetFile.deleteFile(); }

    mName = System::sanitiseFileName (mName, "ChordBro Preset");
    mPresetFileName = mName + PRESET_EXTENSION;
    mIsPresetModified = false;

    auto rootXml = Presets::createPresetRootXml (mChords);
    rootXml->writeTo (PRESET_FOLDER.getChildFile (mPresetFileName));

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFileSaved;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickImportMidi()
{
    FileChooser chooser ("Select a MIDI file(s)...", DESKTOP_FOLDER, "*.mid");

    if (chooser.browseForMultipleFilesToOpen())
    {
        juce::Array<File> chosenFiles = chooser.getResults();

        for (int index = 0; index < chosenFiles.size(); index++)
        {
            File chosenFile = chosenFiles.getUnchecked (index);
            saveMidiFile (chosenFile, index + 1 == chosenFiles.size());
        }
    }
}

void PresetState::handleClickExportMidi()
{
    if (!isPresetValid()) { return; }

    FileChooser chooser ("Export MIDI as...", DESKTOP_FOLDER, "*.mid");

    if (chooser.browseForFileToSave (true))
    {
        MidiFile midiFile;
        chooser.getResult().File::deleteFile();
        FileOutputStream stream (chooser.getResult());
        midiFile.setTicksPerQuarterNote (TICKS_PER_QUARTER_NOTE);
        midiFile.addTrack (Presets::getMidiSequenceFromChords (mChords));
        midiFile.writeTo (stream);
    }
}

void PresetState::handleClickImportPreset()
{
    FileChooser chooser ("Select a preset file(s)...", DESKTOP_FOLDER, "*" + PRESET_EXTENSION);

    if (chooser.browseForMultipleFilesToOpen())
    {
        juce::Array<File> chosenFiles = chooser.getResults();

        for (int index = 0; index < chosenFiles.size(); index++)
        {
            File chosenFile = chosenFiles.getUnchecked (index);
            savePresetFile (chosenFile, index + 1 == chosenFiles.size());
        }
    }
}

void PresetState::handleClickExportPreset()
{
    if (!isPresetValid()) { return; }

    FileChooser chooser ("Export preset as...", DESKTOP_FOLDER, "*" + PRESET_EXTENSION);

    if (chooser.browseForFileToSave (true))
    {
        FileOutputStream stream (chooser.getResult());
        auto rootXml = Presets::createPresetRootXml (mChords);
        rootXml->writeTo (stream);
    }
}

void PresetState::handleClickImportMPC()
{
    FileChooser chooser ("Select a MPC file(s)...", DESKTOP_FOLDER, "*.progression");

    if (chooser.browseForMultipleFilesToOpen())
    {
        juce::Array<File> chosenFiles = chooser.getResults();

        for (int index = 0; index < chosenFiles.size(); index++)
        {
            File chosenFile = chosenFiles.getUnchecked (index);
            saveMPCFile (chosenFile, index + 1 == chosenFiles.size());
        }
    }
}

void PresetState::handleClickDuplicate()
{
    resetPresetState (true);
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFileNew;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickEditLeft()
{
    juce::Array<int> inputNotes = getPresetInputNotes();
    if (inputNotes.isEmpty() || inputNotes.contains (21) || mEditModeInputNote == 21) { return; }

    std::map<int, Chord> nextChords;

    for (int inputNote : inputNotes)
    {
        nextChords[inputNote - 1] = getChord (inputNote);
    }

    const int prevEditModeInputNote = mEditModeInputNote;
    const int nextEditModeInputNote = mEditModeInputNote > 0 ? mEditModeInputNote - 1 : 0;

    mChords.clear();
    mChords = nextChords;
    mEditModeInputNote = nextEditModeInputNote;

    juce::Array<int> prevPresetInputNotes = inputNotes;
    juce::Array<int> nextPresetInputNotes = getPresetInputNotes();

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kEditModeShiftArrow;
    message->messageVar1 = prevEditModeInputNote;
    message->messageVar2 = nextEditModeInputNote;
    message->messageArray1 = prevPresetInputNotes;
    message->messageArray2 = nextPresetInputNotes;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickEditRight()
{
    juce::Array<int> inputNotes = getPresetInputNotes();
    if (inputNotes.isEmpty() || inputNotes.contains (108) || mEditModeInputNote == 108) { return; }

    std::map<int, Chord> nextChords;

    for (int inputNote : inputNotes)
    {
        nextChords[inputNote + 1] = getChord (inputNote);
    }

    const int prevEditModeInputNote = mEditModeInputNote;
    const int nextEditModeInputNote = mEditModeInputNote > 0 ? mEditModeInputNote + 1 : 0;

    mChords.clear();
    mChords = nextChords;
    mEditModeInputNote = nextEditModeInputNote;

    juce::Array<int> prevPresetInputNotes = inputNotes;
    juce::Array<int> nextPresetInputNotes = getPresetInputNotes();

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kEditModeShiftArrow;
    message->messageVar1 = prevEditModeInputNote;
    message->messageVar2 = nextEditModeInputNote;
    message->messageArray1 = prevPresetInputNotes;
    message->messageArray2 = nextPresetInputNotes;
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickAllWhite()
{
    juce::Array<int> inputNotes = getPresetInputNotes();
    if (inputNotes.isEmpty()) { return; }

    inputNotes.sort();
    int lowestNote = inputNotes[0];
    while (Keyboard::isBlackKey (lowestNote)) { lowestNote++; }
    if (lowestNote > 108) { return; }

    int whiteNoteIndex = Keyboard::getWhiteNoteIndex (lowestNote);
    std::map<int, Chord> nextChords;

    for (int inputNote : inputNotes)
    {
        if (whiteNoteIndex > 51) { return; }
        int nextWhiteNote = Keyboard::getWhiteNoteNumber (whiteNoteIndex);
        nextChords[nextWhiteNote] = getChord (inputNote);
        whiteNoteIndex++;
    }

    mChords.clear();
    mChords = nextChords;
    mEditModeInputNote = 0;

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetModified;
    message->messageArray1 = getPresetInputNotes();
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickAllBlack()
{
    juce::Array<int> inputNotes = getPresetInputNotes();
    if (inputNotes.isEmpty()) { return; }

    inputNotes.sort();
    int lowestNote = inputNotes[0];
    while (!Keyboard::isBlackKey (lowestNote)) { lowestNote++; }
    if (lowestNote > 106) { return; }

    int blackNoteIndex = Keyboard::getBlackNoteIndex (lowestNote);
    std::map<int, Chord> nextChords;

    for (int inputNote : inputNotes)
    {
        if (blackNoteIndex > 35) { return; }
        int nextBlackNote = Keyboard::getBlackNoteNumber (blackNoteIndex);
        nextChords[nextBlackNote] = getChord (inputNote);
        blackNoteIndex++;
    }

    mChords.clear();
    mChords = nextChords;
    mEditModeInputNote = 0;

    mIsPresetModified = true;
    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetModified;
    message->messageArray1 = getPresetInputNotes();
    sendMessage (message, ListenerType::kSync);
}

void PresetState::handleClickPreset (File inPresetFile)
{
    if (inPresetFile.existsAsFile())
    {
        loadPresetFile (inPresetFile);
    }
}

//==============================================================================
Chord PresetState::getChord (const int inInputNote)
{
    auto pair = mChords.find (inInputNote);
    if (pair == mChords.end()) { return mEmptyChord; }
    return pair->second;
}

void PresetState::setChord (const int inInputNote, Chord inChord)
{
    mChords[inInputNote] = inChord;
}

void PresetState::resetPresetState (bool inKeepChords)
{
    if (!inKeepChords) { mChords.clear(); }

    mName.clear();
    mPresetFileName.clear();
    mIsPresetModified = false;
    mEditModeInputNote = 0;
}

void PresetState::applyLoadedPreset (const String& inPresetName, const std::map<int, Chord>& inChords)
{
    resetPresetState (false);
    mName = System::sanitiseFileName (inPresetName, "ChordBro Preset");
    mPresetFileName = mName + PRESET_EXTENSION;
    mChords = inChords;

    DataMessage* message = new DataMessage();
    message->messageCode = MessageCode::kPresetFileLoaded;
    message->messageVar1 = mName;
    message->messageArray1 = getPresetInputNotes();
    sendMessage (message, ListenerType::kSync);
}

bool PresetState::persistPresetFile (const String& inPresetName, const std::map<int, Chord>& inChords)
{
    if (!System::ensureDirectoryExists (PRESET_FOLDER))
    {
        return false;
    }

    const String presetName = System::sanitiseFileName (inPresetName, "ChordBro Preset");
    const File presetFile = PRESET_FOLDER.getChildFile (presetName + PRESET_EXTENSION);

    if (presetFile.existsAsFile())
    {
        presetFile.deleteFile();
    }

    auto rootXml = Presets::createPresetRootXml (inChords);
    return rootXml->writeTo (presetFile);
}

void PresetState::showImportError (const String& inTitle, const File& inSourceFile, const String& inReason)
{
    String message = inReason;

    if (inSourceFile.existsAsFile())
    {
        message << "\n\nSource: " << inSourceFile.getFullPathName();
    }

    System::showMessage (AlertWindow::WarningIcon, inTitle, message);
}

//==============================================================================
void PresetState::loadPresetFile (File inPresetFile)
{
    PresetImportResult result = Presets::readPresetFile (inPresetFile);

    if (!result.succeeded())
    {
        showImportError ("Preset import failed", inPresetFile, result.errorMessage);
        return;
    }

    applyLoadedPreset (inPresetFile.getFileNameWithoutExtension(), result.chords);
}

void PresetState::loadMidiFile (File inMidiFile)
{
    saveMidiFile (inMidiFile, true);
}

void PresetState::loadMPCFile (File inMPCFile)
{
    saveMPCFile (inMPCFile, true);
}

//==============================================================================
bool PresetState::savePresetFile (File inPresetFile, bool inShouldLoadAfterImport)
{
    PresetImportResult result = Presets::readPresetFile (inPresetFile);

    if (!result.succeeded())
    {
        showImportError ("Preset import failed", inPresetFile, result.errorMessage);
        return false;
    }

    const String presetName = System::sanitiseFileName (inPresetFile.getFileNameWithoutExtension(), "ChordBro Preset");

    if (!persistPresetFile (presetName, result.chords))
    {
        showImportError ("Preset import failed", inPresetFile, "ChordBro could not save the imported preset.");
        return false;
    }

    if (inShouldLoadAfterImport)
    {
        applyLoadedPreset (presetName, result.chords);
    }

    return true;
}

bool PresetState::saveMidiFile (File inMidiFile, bool inShouldLoadAfterImport)
{
    PresetImportResult result = Presets::readMidiFile (inMidiFile);

    if (!result.succeeded())
    {
        showImportError ("MIDI import failed", inMidiFile, result.errorMessage);
        return false;
    }

    const String presetName = System::sanitiseFileName (inMidiFile.getFileNameWithoutExtension(), "ChordBro MIDI Import");

    if (!persistPresetFile (presetName, result.chords))
    {
        showImportError ("MIDI import failed", inMidiFile, "ChordBro could not save the imported MIDI as a preset.");
        return false;
    }

    if (inShouldLoadAfterImport)
    {
        applyLoadedPreset (presetName, result.chords);
    }

    return true;
}

bool PresetState::saveMPCFile (File inMPCFile, bool inShouldLoadAfterImport)
{
    PresetImportResult result = Presets::readMPCFile (inMPCFile);

    if (!result.succeeded())
    {
        showImportError ("MPC import failed", inMPCFile, result.errorMessage);
        return false;
    }

    const String presetName = System::sanitiseFileName (inMPCFile.getFileNameWithoutExtension(), "ChordBro MPC Import");

    if (!persistPresetFile (presetName, result.chords))
    {
        showImportError ("MPC import failed", inMPCFile, "ChordBro could not save the imported MPC progression.");
        return false;
    }

    if (inShouldLoadAfterImport)
    {
        applyLoadedPreset (presetName, result.chords);
    }

    return true;
}

//==============================================================================
XmlElement* PresetState::exportPresetStateXml()
{
    XmlElement* presetStateXml = new XmlElement ("PresetState");
    presetStateXml->setAttribute ("name", mName);
    presetStateXml->setAttribute ("presetFileName", mPresetFileName);
    presetStateXml->setAttribute ("isPresetModified", mIsPresetModified);
    presetStateXml->addChildElement (Presets::getPresetXmlFromChords (mChords));
    return presetStateXml;
}

void PresetState::importPresetStateXml (XmlElement* inPresetStateXml)
{
    if (inPresetStateXml == nullptr) { return; }

    XmlElement* presetXml = inPresetStateXml->getFirstChildElement();
    mName = inPresetStateXml->getStringAttribute ("name");
    mPresetFileName = inPresetStateXml->getStringAttribute ("presetFileName");
    mIsPresetModified = inPresetStateXml->getBoolAttribute ("isPresetModified");
    mChords = Presets::getChordsFromPresetXml (presetXml);
}
