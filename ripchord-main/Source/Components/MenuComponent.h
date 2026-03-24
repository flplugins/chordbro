#pragma once

#include "JuceHeader.h"
#include "Styles.h"
#include "MainProcess.h"
#include "DataMessageListener.h"

//==============================================================================
class MenuComponent : public Component, public DataMessageListener
{
public:
    //==============================================================================
    MenuComponent (MainProcess&);
    ~MenuComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseDown (const MouseEvent& event) override;

    //==============================================================================
    void handleNewMessage (const DataMessage* message) override;

private:
    //==============================================================================
    MainProcess& mMainProcess;
    GlobalState& mGlobalState;
    PresetState& mPresetState;

    Label mUtilityLabel { "", "UTILITY" };
    Label mActionsLabel { "", "FILE TOOLS" };
    Label mCaptionLabel { "", "Official FL Plugins build." };
    TextButton mHelpButton { "Help & Updates" };
    TextButton mResetUtilityButton { "Reset Size" };
    TextButton mExportMidiButton { "Export Current MIDI" };
    TextButton mImportPresetButton { "Import Preset File" };

    //==============================================================================
    void handleToggleMenu (const DataMessage* message);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuComponent)
};
