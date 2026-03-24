#include "KeyboardViewComponent.h"

namespace
{
    constexpr float kFooterFieldX = SPACE;
    constexpr float kFooterFieldWidth = KEYBOARD_BG_WIDTH - BUTTON_WIDTH - HALF_SPACE;
}

//==============================================================================
KeyboardViewComponent::KeyboardViewComponent (MainProcess& inMainProcess)
:   mMainProcess (inMainProcess),
    mGlobalState (mMainProcess.getGlobalState()),
    mBrowserState (mMainProcess.getBrowserState()),
    mPresetState (mMainProcess.getPresetState()),
    mMidiState (mMainProcess.getMidiState()),
    mOutputKeyboard (inMainProcess),
    mInputKeyboard (inMainProcess),
    mPlayControls (inMainProcess),
    mEditControls (inMainProcess),
    mPresetName (inMainProcess),
    mChordName (inMainProcess)
{
    mGlobalState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);

    setWantsKeyboardFocus (true);

    mImages.setDrawableButtonImages (mMenuButton, "GearCircle.svg");
    mImages.setDrawableButtonImages (mPowerButton, mGlobalState.isPowerOn() ? "PowerON.svg" : "Power.svg");
    mImages.setDrawableButtonImages (mPresetsButton, "Presets.svg");

    mOutputKeyboardLabel.setText ("OUTPUT STACK", dontSendNotification);
    mInputKeyboardLabel.setText ("TRIGGER KEYS", dontSendNotification);
    mOutputKeyboardLabel.setColour (Label::textColourId, COLOR_GOLD);
    mInputKeyboardLabel.setColour (Label::textColourId, COLOR_WHITE.withAlpha (0.8f));

    mMenuButton.setTriggeredOnMouseDown (true);
    mMenuButton.onClick = [this]() { mGlobalState.toggleMenu(); };

    mPowerButton.setTriggeredOnMouseDown (true);
    mPowerButton.onClick = [this]()
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return; }
        if (mMidiState.isRecording()) { return; }
        mGlobalState.togglePower();
    };

    mPresetsButton.setTriggeredOnMouseDown (true);
    mPresetsButton.onClick = [this]()
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return; }
        if (mMidiState.isRecording()) { return; }
        mGlobalState.toggleView();
    };

    mOutputKeyboard.setBounds (KEYBOARD_X, OUTPUT_KEYBOARD_Y, KEYBOARD_WIDTH, KEYBOARD_HEIGHT);
    mInputKeyboard.setBounds (KEYBOARD_X, INPUT_KEYBOARD_Y, KEYBOARD_WIDTH, KEYBOARD_HEIGHT);
    mPlayControls.setBounds (SPACE, CONTROLS_Y, CONTROLS_WIDTH, CONTROLS_HEIGHT);
    mEditControls.setBounds (SPACE, CONTROLS_Y, CONTROLS_WIDTH, CONTROLS_HEIGHT);
    mPresetName.setBounds (kFooterFieldX, FOOTER_Y, kFooterFieldWidth, ITEM_HEIGHT);
    mChordName.setBounds (TEXT_INPUT_X, HEADER_Y, TEXT_INPUT_WIDTH, ITEM_HEIGHT);

    addAndMakeVisible (mInputKeyboardLabel);
    addAndMakeVisible (mOutputKeyboardLabel);

    addAndMakeVisible (mMenuButton);
    addAndMakeVisible (mPowerButton);
    addAndMakeVisible (mPresetsButton);

    addAndMakeVisible (mOutputKeyboard);
    addAndMakeVisible (mInputKeyboard);
    addAndMakeVisible (mPlayControls);
    addChildComponent (mEditControls);
    addAndMakeVisible (mPresetName);
    addAndMakeVisible (mChordName);
}

KeyboardViewComponent::~KeyboardViewComponent()
{
}

//==============================================================================
void KeyboardViewComponent::paint (Graphics& inGraphics)
{
    auto mainArea = getLocalBounds();
    auto outputPanel = Styles::getRelativeBounds (mainArea, SPACE, OUTPUT_KEYBOARD_BG_Y, KEYBOARD_BG_WIDTH, KEYBOARD_BG_HEIGHT);
    auto inputPanel = Styles::getRelativeBounds (mainArea, SPACE, INPUT_KEYBOARD_BG_Y, KEYBOARD_BG_WIDTH, KEYBOARD_BG_HEIGHT);
    auto controlsPanel = Styles::getRelativeBounds (mainArea, SPACE, CONTROLS_Y, CONTROLS_WIDTH, CONTROLS_HEIGHT);
    auto topFieldPanel = Styles::getRelativeBounds (mainArea, TEXT_INPUT_X, HEADER_Y, TEXT_INPUT_WIDTH, ITEM_HEIGHT);
    auto bottomFieldPanel = Styles::getRelativeBounds (mainArea, kFooterFieldX, FOOTER_Y, kFooterFieldWidth, ITEM_HEIGHT);
    auto actionRail = mMenuButton.getBounds().getUnion (mPowerButton.getBounds()).expanded (12, 8).toFloat();

    Styles::drawPanel (inGraphics, outputPanel.toFloat(), 26.f, COLOR_PANEL_RAISED.brighter (0.02f), 0.76f);
    Styles::drawPanel (inGraphics, inputPanel.toFloat(), 26.f, COLOR_PANEL.brighter (0.02f), 0.72f);
    Styles::drawPanel (inGraphics, controlsPanel.toFloat(), 20.f, COLOR_PANEL_SOFT.brighter (0.04f), 0.66f);
    Styles::drawInsetField (inGraphics, topFieldPanel.toFloat(), 17.f, COLOR_GOLD, 0.2f);
    Styles::drawInsetField (inGraphics, bottomFieldPanel.toFloat(), 17.f, COLOR_BLUE, 0.24f);
    Styles::drawPanel (inGraphics, actionRail, actionRail.getHeight() * 0.5f, COLOR_PANEL_RAISED.withAlpha (0.96f), 0.54f);

    auto outputAccent = outputPanel.toFloat();
    outputAccent.setHeight (3.f);
    inGraphics.setColour (COLOR_BLUE.withAlpha (0.24f));
    inGraphics.fillRoundedRectangle (outputAccent, 24.f);

    auto inputAccent = inputPanel.toFloat();
    inputAccent.setHeight (3.f);
    inGraphics.setColour (COLOR_GOLD.withAlpha (0.22f));
    inGraphics.fillRoundedRectangle (inputAccent, 24.f);
}

void KeyboardViewComponent::resized()
{
    auto mainArea = getLocalBounds();

    mOutputKeyboardLabel.setBounds (Styles::getRelativeBounds (mainArea, KEYBOARD_LABEL_X, OUTPUT_LABEL_Y,
                                                               KEYBOARD_LABEL_WIDTH, KEYBOARD_LABEL_HEIGHT));

    mInputKeyboardLabel.setBounds (Styles::getRelativeBounds (mainArea, KEYBOARD_LABEL_X, INPUT_LABEL_Y,
                                                              KEYBOARD_LABEL_WIDTH, KEYBOARD_LABEL_HEIGHT));

    auto keyboardLabelFont = Styles::getAccentFont ((float) mOutputKeyboardLabel.getHeight() - 2.f, true, 0.1f);
    mOutputKeyboardLabel.setFont (keyboardLabelFont);
    mInputKeyboardLabel.setFont (Styles::getAccentFont ((float) mInputKeyboardLabel.getHeight() - 3.f, true, 0.08f));

    float scaleFactor = (float) getWidth() / EDITOR_WIDTH;
    mOutputKeyboard.setTransform (AffineTransform::scale (scaleFactor));
    mInputKeyboard.setTransform (AffineTransform::scale (scaleFactor));
    mPlayControls.setTransform (AffineTransform::scale (scaleFactor));
    mEditControls.setTransform (AffineTransform::scale (scaleFactor));
    mPresetName.setTransform (AffineTransform::scale (scaleFactor));
    mChordName.setTransform (AffineTransform::scale (scaleFactor));

    const float topButtonGap = HALF_SPACE + 6.f;
    const float topPowerX = POWER_BUTTON_X;
    const float topMenuX = topPowerX - ITEM_HEIGHT - topButtonGap;

    mMenuButton.setBounds (Styles::getRelativeBounds (mainArea, topMenuX, HEADER_Y, ITEM_HEIGHT, ITEM_HEIGHT));
    mPowerButton.setBounds (Styles::getRelativeBounds (mainArea, topPowerX, HEADER_Y, ITEM_HEIGHT, ITEM_HEIGHT));
    mPresetsButton.setBounds (Styles::getRelativeBounds (mainArea, RIGHT_BUTTON_X, FOOTER_Y, BUTTON_WIDTH, ITEM_HEIGHT));
}

//==============================================================================
bool KeyboardViewComponent::keyPressed (const KeyPress& inKey)
{
    auto keyCode = inKey.getKeyCode();
    bool isLeftArrow = keyCode == KeyPress::leftKey || keyCode == KeyPress::upKey;
    bool isRightArrow = keyCode == KeyPress::rightKey || keyCode == KeyPress::downKey;
    bool isTargetKey = isLeftArrow || isRightArrow;

    if (!isTargetKey || mGlobalState.isPresetView() || mGlobalState.isEditMode()) { return false; }

    if (isLeftArrow)
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return false; }
        mBrowserState.handleClickLeftArrow (mPresetState.getName());
    }

    if (isRightArrow)
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return false; }
        mBrowserState.handleClickRightArrow (mPresetState.getName());
    }

    return true;
}

//==============================================================================
void KeyboardViewComponent::handleNewMessage (const DataMessage* inMessage)
{
    switch (inMessage->messageCode)
    {
        case (MessageCode::kTogglePower): { handleTogglePower (inMessage); } break;
        default: { } break;
    };
}

void KeyboardViewComponent::handleTogglePower (const DataMessage* inMessage)
{
    mImages.setDrawableButtonImages (mPowerButton, mGlobalState.isPowerOn() ? "PowerON.svg" : "Power.svg");
}
