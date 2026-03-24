#pragma once

#include "Midi.h"
#include "System.h"
#include "Keyboard.h"

//==============================================================================
const String PRESET_EXTENSION = ".rpc";

//==============================================================================
struct Preset
{
    int indexValue;
    String fileName;
    bool isFavorite;
};

//==============================================================================
struct Chord
{
    String name;
    juce::Array<int> notes;
};

//==============================================================================
struct PresetImportResult
{
    std::map<int, Chord> chords;
    String errorMessage;
    String warningMessage;

    bool succeeded() const
    {
        return errorMessage.isEmpty() && !chords.empty();
    }
};

//==============================================================================
namespace Presets
{
    static constexpr int kMinMidiNote = 21;
    static constexpr int kMaxMidiNote = 108;
    static constexpr int kMaxNotesPerChord = 12;

    //==============================================================================
    static inline Array<File> getPresetLibraryFolders()
    {
        Array<File> folders;
        System::ensureDirectoryExists (PRESET_FOLDER);
        folders.addIfNotAlreadyThere (PRESET_FOLDER);

        if (PREMIUM_PACKS_FOLDER.isDirectory())
        {
            folders.addIfNotAlreadyThere (PREMIUM_PACKS_FOLDER);
        }

        return folders;
    }

    static inline Array<File> getSortedPresetFiles()
    {
        Array<File> files;

        for (const auto& folder : getPresetLibraryFolders())
        {
            files.addArray (folder.findChildFiles (File::findFiles, true, "*" + PRESET_EXTENSION));
        }

        files.sort();
        return files;
    }

    static inline File findPresetFileByName (const String& inPresetName)
    {
        for (const auto& file : getSortedPresetFiles())
        {
            if (file.getFileNameWithoutExtension() == inPresetName)
            {
                return file;
            }
        }

        return {};
    }

    static inline bool isInstalledPremiumPreset (const File& inPresetFile)
    {
        return inPresetFile == PREMIUM_PACKS_FOLDER || inPresetFile.isAChildOf (PREMIUM_PACKS_FOLDER);
    }

    static inline bool isValidMidiNote (const int inNoteNumber)
    {
        return inNoteNumber >= kMinMidiNote && inNoteNumber <= kMaxMidiNote;
    }

    static inline juce::Array<int> sanitiseChordNotes (juce::Array<int> inNotes)
    {
        juce::Array<int> sanitizedNotes;

        for (const int note : inNotes)
        {
            if (isValidMidiNote (note))
            {
                sanitizedNotes.addIfNotAlreadyThere (note);
            }
        }

        sanitizedNotes.sort();

        while (sanitizedNotes.size() > kMaxNotesPerChord)
        {
            sanitizedNotes.removeLast();
        }

        return sanitizedNotes;
    }

    static inline String generateNameFromNotes (juce::Array<int> inNotes)
    {
        String chordName = "";

        for (int index = 0; index < inNotes.size(); index++)
        {
            if ((index + 1) == inNotes.size())
            {
                chordName << Keyboard::getNoteNameFromNumber (inNotes[index]);
            }
            else
            {
                chordName << Keyboard::getNoteNameFromNumber (inNotes[index]) << " ";
            }
        }

        return chordName;
    }

    static inline String sanitiseChordName (String inName, const juce::Array<int>& inNotes)
    {
        String chordName = inName.trim();

        if (chordName.isEmpty())
        {
            chordName = generateNameFromNotes (inNotes);
        }

        chordName = chordName.substring (0, 80).trim();
        return chordName.isNotEmpty() ? chordName : "Chord";
    }

    static inline XmlElement* getPresetXmlFromChords (std::map<int, Chord> inChords)
    {
        XmlElement* presetXml = new XmlElement ("preset");

        for (const auto& pair : inChords)
        {
            StringArray chordNotes;
            juce::Array<int> sortedNotes = sanitiseChordNotes (pair.second.notes);

            if (!isValidMidiNote (pair.first) || sortedNotes.isEmpty())
            {
                continue;
            }

            for (const int chordNote : sortedNotes)
            {
                chordNotes.add (String (chordNote));
            }

            XmlElement* inputXml = new XmlElement ("input");
            XmlElement* chordXml = new XmlElement ("chord");

            inputXml->setAttribute ("note", String (pair.first));
            chordXml->setAttribute ("name", sanitiseChordName (pair.second.name, sortedNotes));
            chordXml->setAttribute ("notes", chordNotes.joinIntoString (";"));

            inputXml->addChildElement (chordXml);
            presetXml->addChildElement (inputXml);
        }

        return presetXml;
    }

    static inline std::unique_ptr<XmlElement> createPresetRootXml (const std::map<int, Chord>& inChords)
    {
        auto rootXml = std::make_unique<XmlElement> ("ripchord");
        rootXml->setAttribute ("product", "ChordBro");
        rootXml->setAttribute ("manufacturer", "FL Plugins");
        rootXml->setAttribute ("schemaVersion", 2);
        rootXml->addChildElement (getPresetXmlFromChords (inChords));
        return rootXml;
    }

    static inline MidiMessageSequence getMidiSequenceFromChords (std::map<int, Chord> inChords)
    {
        float ticks = 0.0f;
        MidiMessageSequence sequence;

        for (const auto& pair : inChords)
        {
            juce::Array<int> notes = pair.second.notes;

            for (const int chordNote : notes)
            {
                const auto& noteOn = MidiMessage::noteOn (1, chordNote, 0.8f);
                const auto& noteOff = MidiMessage::noteOff (1, chordNote);
                sequence.addEvent (noteOn, ticks);
                sequence.addEvent (noteOff, ticks + TICKS_PER_QUARTER_NOTE / 2);
            }

            ticks = ticks + TICKS_PER_QUARTER_NOTE;
        }

        return sequence;
    }

    static inline PresetImportResult parsePresetXml (const XmlElement* inPresetXml)
    {
        PresetImportResult result;
        std::map<int, Chord> chords;

        if (inPresetXml == nullptr || !inPresetXml->hasTagName ("preset"))
        {
            result.errorMessage = "The preset file is missing its <preset> payload.";
            return result;
        }

        StringArray warnings;

        for (const XmlElement* inputXml : inPresetXml->getChildWithTagNameIterator ("input"))
        {
            if (inputXml == nullptr)
            {
                continue;
            }

            Chord chord;
            juce::Array<int> notes;
            const String noteString = inputXml->getStringAttribute ("note").trim();

            if (noteString.isEmpty() || !noteString.containsOnly ("0123456789"))
            {
                warnings.add ("Skipped an input without a valid trigger note.");
                continue;
            }

            const int note = noteString.getIntValue();

            if (!isValidMidiNote (note))
            {
                warnings.add ("Skipped a trigger note outside the supported MIDI range.");
                continue;
            }

            const XmlElement* chordXml = inputXml->getChildByName ("chord");

            if (chordXml == nullptr)
            {
                warnings.add ("Skipped an input that did not define a chord.");
                continue;
            }

            const String name = chordXml->getStringAttribute ("name");
            const String notesString = chordXml->getStringAttribute ("notes");
            StringArray notesSA = StringArray::fromTokens (notesString, ";", "");

            for (String& noteToken : notesSA)
            {
                const String trimmedToken = noteToken.trim();

                if (trimmedToken.isNotEmpty() && trimmedToken.containsOnly ("0123456789"))
                {
                    notes.add (trimmedToken.getIntValue());
                }
            }

            notes = sanitiseChordNotes (notes);

            if (!notes.isEmpty())
            {
                chord.name = sanitiseChordName (name, notes);
                chord.notes = notes;
                chords[note] = chord;
            }
        }

        if (chords.empty())
        {
            result.errorMessage = "No valid chords were found in the preset.";
            return result;
        }

        result.chords = chords;
        result.warningMessage = warnings.joinIntoString (" ");
        return result;
    }

    static inline std::map<int, Chord> getChordsFromPresetXml (XmlElement* inPresetXml)
    {
        return parsePresetXml (inPresetXml).chords;
    }

    static inline PresetImportResult readPresetFile (const File& inPresetFile)
    {
        PresetImportResult result;

        if (!inPresetFile.existsAsFile())
        {
            result.errorMessage = "The selected preset file could not be found.";
            return result;
        }

        std::unique_ptr<XmlElement> rootXml = parseXML (inPresetFile);

        if (rootXml == nullptr)
        {
            result.errorMessage = "The preset file is not valid XML.";
            return result;
        }

        const XmlElement* presetXml = rootXml->hasTagName ("preset") ? rootXml.get() : rootXml->getChildByName ("preset");
        return parsePresetXml (presetXml);
    }

    static inline PresetImportResult readMidiFile (const File& inMidiFile)
    {
        PresetImportResult result;
        int noteNumber = 60;
        juce::Array<String> chordNames;
        std::map<int, Chord> chords;
        juce::Array<int> activeNotes;
        juce::Array<int> currentChordNotes;

        MidiFile midiFile;
        MidiMessageSequence events;
        FileInputStream midiFileStream (inMidiFile);

        if (!inMidiFile.existsAsFile() || !midiFileStream.openedOk() || !midiFile.readFrom (midiFileStream))
        {
            result.errorMessage = "The MIDI file could not be opened or parsed.";
            return result;
        }

        const int largeNumberOfEvents = 160;
        const int numberOfTracks = midiFile.getNumTracks();

        if (numberOfTracks <= 0)
        {
            result.errorMessage = "The MIDI file does not contain any tracks.";
            return result;
        }

        for (int index = 0; index < numberOfTracks; index++)
        {
            const MidiMessageSequence* midiTrack = midiFile.getTrack (index);

            if (midiTrack == nullptr)
            {
                continue;
            }

            for (MidiMessageSequence::MidiEventHolder* event : *midiTrack)
            {
                if (event != nullptr && event->message.isNoteOnOrOff())
                {
                    events.addEvent (event->message, event->message.getTimeStamp());
                }
            }
        }

        events.sort();

        if (events.getNumEvents() > largeNumberOfEvents)
        {
            noteNumber = kMinMidiNote;
        }

        for (MidiMessageSequence::MidiEventHolder* event : events)
        {
            if (event == nullptr || noteNumber > kMaxMidiNote)
            {
                continue;
            }

            const MidiMessage message = event->message;
            const int currentNote = message.getNoteNumber();

            if (!isValidMidiNote (currentNote))
            {
                continue;
            }

            if (message.isNoteOn())
            {
                activeNotes.addIfNotAlreadyThere (currentNote);
                currentChordNotes.addIfNotAlreadyThere (currentNote);
            }

            if (message.isNoteOff())
            {
                activeNotes.removeFirstMatchingValue (currentNote);

                if (activeNotes.isEmpty() && !currentChordNotes.isEmpty())
                {
                    currentChordNotes = sanitiseChordNotes (currentChordNotes);

                    if (!currentChordNotes.isEmpty())
                    {
                        const String chordName = sanitiseChordName ("", currentChordNotes);

                        if (!chordNames.contains (chordName))
                        {
                            Chord chord;
                            chord.name = chordName;
                            chord.notes = currentChordNotes;
                            chordNames.add (chordName);
                            chords[noteNumber] = chord;
                            noteNumber = noteNumber + 1;
                        }
                    }

                    currentChordNotes.clear();
                }
            }
        }

        if (chords.empty())
        {
            result.errorMessage = "No usable chord stacks were found in the MIDI file.";
            return result;
        }

        result.chords = chords;
        return result;
    }

    static inline PresetImportResult readMPCFile (const File& inMPCFile)
    {
        PresetImportResult result;
        int noteNumber = 60;
        std::map<int, Chord> chords;
        var json = JSON::parse (inMPCFile);

        if (json.isVoid() || !json.isObject())
        {
            result.errorMessage = "The MPC progression file is not valid JSON.";
            return result;
        }

        const var progression = json.getProperty ("progression", var());

        if (progression.isVoid() || !progression.isObject())
        {
            result.errorMessage = "The MPC file is missing its progression payload.";
            return result;
        }

        auto jsonChords = progression.getProperty ("chords", var()).getArray();

        if (jsonChords == nullptr || jsonChords->isEmpty())
        {
            result.errorMessage = "The MPC file does not contain any chords.";
            return result;
        }

        int chordCount = jsonChords->size();

        if (chordCount > 40) { noteNumber = 48; }
        if (chordCount > 52) { noteNumber = 36; }
        if (chordCount > 64) { noteNumber = 24; }

        for (auto jsonChord : *jsonChords)
        {
            if (noteNumber > kMaxMidiNote || !jsonChord.isObject())
            {
                continue;
            }

            Chord chord;
            juce::Array<int> notes;
            String name = jsonChord["name"].toString();
            auto* jsonNotes = jsonChord["notes"].getArray();

            if (jsonNotes == nullptr)
            {
                continue;
            }

            for (auto note : *jsonNotes)
            {
                notes.add (note.toString().getIntValue());
            }

            notes = sanitiseChordNotes (notes);

            if (!notes.isEmpty())
            {
                chord.name = sanitiseChordName (name, notes);
                chord.notes = notes;
                chords[noteNumber] = chord;
                noteNumber = noteNumber + 1;
            }
        }

        if (chords.empty())
        {
            result.errorMessage = "No valid chords were found in the MPC progression file.";
            return result;
        }

        result.chords = chords;
        return result;
    }

    static inline std::map<int, Chord> getChordsFromMidiFile (File inMidiFile)
    {
        return readMidiFile (inMidiFile).chords;
    }

    static inline std::map<int, Chord> getChordsFromMPCFile (File inMPCFile)
    {
        return readMPCFile (inMPCFile).chords;
    }
}
