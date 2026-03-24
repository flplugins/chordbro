#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "MainComponent.h"

//==============================================================================
class RipchordPluginEditor : public AudioProcessorEditor
{
public:
    //==============================================================================
    RipchordPluginEditor (RipchordPluginProcessor&);
    ~RipchordPluginEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    MainComponent mMainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RipchordPluginEditor)
};
