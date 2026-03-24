#pragma once

#include "Presets.h"

namespace FactoryPresets
{
    struct ChordDefinition
    {
        int inputNote;
        const char* name;
        std::initializer_list<int> notes;
    };

    struct PresetDefinition
    {
        const char* fileName;
        std::initializer_list<ChordDefinition> chords;
    };

    static inline std::vector<PresetDefinition> getFactoryPresetDefinitions()
    {
        return {};
    }

    static inline StringArray getRetiredFactoryPresetNames()
    {
        return
        {
            "Starter - Sunset Major",
            "Starter - Midnight Minor",
            "Pop - Skyline Hooks",
            "Trap - Velvet Steps",
            "R&B - Silk Room",
            "Dark - Hollow Nights",
            "House - Glass Stabs",
            "Cinematic - Horizon Tension",
            "Jazz - Color Drift",
            "Gospel - Sunday Lift",
            "Drill - Iron Resolve",
            "Bright - Morning Lift",
            "Lo-Fi - Tape Drift",
            "Afro - Sunset Bounce",
            "EDM - Skyline Lift",
            "Soul - After Hours",
            "Advanced - Split Tension",
            "Advanced - Open Bloom"
        };
    }

    static inline StringPairArray getLegacyFactoryPresetNameMappings()
    {
        StringPairArray mappings;

        for (const auto& definition : getFactoryPresetDefinitions())
        {
            const String currentName = definition.fileName;
            const String legacyName = currentName == "R&B - Silk Room" ? "CB RnB - Silk Room"
                                                                         : "CB " + currentName;
            mappings.set (legacyName, currentName);
        }

        return mappings;
    }

    static inline std::map<int, Chord> createChordMap (const PresetDefinition& inDefinition)
    {
        std::map<int, Chord> chords;

        for (const auto& definition : inDefinition.chords)
        {
            Chord chord;

            for (const int note : definition.notes)
            {
                chord.notes.add (note);
            }

            chord.notes = Presets::sanitiseChordNotes (chord.notes);
            chord.name = Presets::sanitiseChordName (definition.name, chord.notes);

            if (Presets::isValidMidiNote (definition.inputNote) && !chord.notes.isEmpty())
            {
                chords[definition.inputNote] = chord;
            }
        }

        return chords;
    }

    static inline void ensureFactoryPresetLibraryInstalled()
    {
        if (!System::ensureDirectoryExists (PRESET_FOLDER))
        {
            return;
        }

        for (const auto& definition : getFactoryPresetDefinitions())
        {
            const String presetName = System::sanitiseFileName (definition.fileName, "ChordBro Factory");
            const File presetFile = PRESET_FOLDER.getChildFile (presetName + PRESET_EXTENSION);

            if (presetFile.existsAsFile())
            {
                continue;
            }

            auto rootXml = Presets::createPresetRootXml (createChordMap (definition));
            rootXml->writeTo (presetFile);
        }
    }
}
