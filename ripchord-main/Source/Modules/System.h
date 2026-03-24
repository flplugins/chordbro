#pragma once

//==============================================================================
namespace System
{
    //==============================================================================
    static inline String sanitiseFileName (String inFileName, String inFallback = "ChordBro Preset")
    {
        const String valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 #_-()[]&+!,'";
        String sanitized = inFileName.trim();

        if (sanitized.isEmpty())
        {
            sanitized = inFallback;
        }

        sanitized = sanitized.replaceCharacters ("\\/:*?\"<>|", "_________");
        sanitized = sanitized.retainCharacters (valid);

        while (sanitized.contains ("  "))
        {
            sanitized = sanitized.replace ("  ", " ");
        }

        sanitized = sanitized.trim();

        if (sanitized.isEmpty())
        {
            sanitized = inFallback.retainCharacters (valid).trim();
        }

        if (sanitized.length() > 64)
        {
            sanitized = sanitized.substring (0, 64).trim();
        }

        return sanitized.isNotEmpty() ? sanitized : "ChordBro Preset";
    }

    static inline bool isValidFileName (String inFileName)
    {
        return sanitiseFileName (inFileName) == inFileName.trim();
    }

    static inline File getUserDataPath (StringRef inCompanyName, StringRef inAppName)
    {
        #if JUCE_MAC || JUCE_IOS
            File directory ("~/Library/");
            directory = directory.getChildFile ("Application Support");
        #elif JUCE_LINUX || JUCE_ANDROID
            const File directory ("~");
        #elif JUCE_WINDOWS
            File directory (File::getSpecialLocation (File::userApplicationDataDirectory));
        #endif

        return directory.getChildFile (inCompanyName).getChildFile (inAppName);
    }

    static inline File getSharedDataPath (StringRef inCompanyName, StringRef inAppName)
    {
        File directory (File::getSpecialLocation (File::commonApplicationDataDirectory));
        return directory.getChildFile (inCompanyName).getChildFile (inAppName);
    }

    static inline bool ensureDirectoryExists (const File& inDirectory)
    {
        return inDirectory.isDirectory() || inDirectory.createDirectory();
    }

    static inline String getLeafName (const String& inPath)
    {
        const int slashIndex = jmax (inPath.lastIndexOfChar ('/'), inPath.lastIndexOfChar ('\\'));
        return slashIndex >= 0 ? inPath.substring (slashIndex + 1) : inPath;
    }

    static inline void showMessage (AlertWindow::AlertIconType inIconType, const String& inTitle, const String& inMessage)
    {
        NativeMessageBox::showMessageBoxAsync (inIconType, inTitle, inMessage);
    }

    static inline PropertiesFile::Options createPluginPropertiesOptions (const String& inFilenameSuffix)
    {
        PropertiesFile::Options pluginPropertiesOptions;
        pluginPropertiesOptions.applicationName = JucePlugin_Name;
        pluginPropertiesOptions.folderName = File::addTrailingSeparator (JucePlugin_Manufacturer) + JucePlugin_Name;
        pluginPropertiesOptions.filenameSuffix = inFilenameSuffix;
        pluginPropertiesOptions.osxLibrarySubFolder = "Application Support";
        pluginPropertiesOptions.commonToAllUsers = false;
        pluginPropertiesOptions.ignoreCaseOfKeyNames = true;
        pluginPropertiesOptions.doNotSave = false;
        pluginPropertiesOptions.millisecondsBeforeSaving = 10;
        pluginPropertiesOptions.storageFormat = PropertiesFile::storeAsXML;
        return pluginPropertiesOptions;
    }
}

const String TEMP_FILE_NAME = "ChordBro.mid";
const File TEMP_FOLDER = File::getSpecialLocation (File::SpecialLocationType::tempDirectory);
const File DESKTOP_FOLDER = File::getSpecialLocation (File::userDesktopDirectory);
const File CHORDBRO_FOLDER = System::getUserDataPath (ProjectInfo::companyName, ProjectInfo::projectName);
const File CHORDBRO_SHARED_FOLDER = System::getSharedDataPath (ProjectInfo::companyName, ProjectInfo::projectName);
const File PRESET_FOLDER = CHORDBRO_FOLDER.getChildFile ("Presets");
const File PREMIUM_PACKS_FOLDER = CHORDBRO_SHARED_FOLDER.getChildFile ("Premium Packs");
