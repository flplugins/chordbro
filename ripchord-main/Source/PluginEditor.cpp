#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RipchordPluginEditor::RipchordPluginEditor (RipchordPluginProcessor& inRipchordPluginProcessor)
:   AudioProcessorEditor (&inRipchordPluginProcessor),
    mMainComponent (inRipchordPluginProcessor.getMainProcess())
{
    setOpaque (true);
    setWantsKeyboardFocus (false);
    setMouseClickGrabsKeyboardFocus (false);
    setSize (EDITOR_WIDTH, EDITOR_HEIGHT);
    addAndMakeVisible (mMainComponent);
}

RipchordPluginEditor::~RipchordPluginEditor()
{
}

//==============================================================================
void RipchordPluginEditor::paint (Graphics& inGraphics)
{
    ignoreUnused (inGraphics);
}

void RipchordPluginEditor::resized()
{
    mMainComponent.setBounds (getLocalBounds());
}
