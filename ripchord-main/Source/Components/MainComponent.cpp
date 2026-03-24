#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent (MainProcess& inMainProcess)
:   mMainProcess (inMainProcess),
    mGlobalState (mMainProcess.getGlobalState()),
    mPresetState (mMainProcess.getPresetState()),
    mKeyboardViewComponent (inMainProcess),
    mPresetViewComponent (inMainProcess),
    mRightClickComponent (inMainProcess),
    mMenuComponent (inMainProcess)
{
    mGlobalState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);
    mPresetState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);

    mGlobalState.setThemeFromConfig();

    setOpaque (true);
    mTitleLabel.setJustificationType (Justification::centredLeft);
    mTitleLabel.setColour (Label::textColourId, COLOR_WHITE.withBrightness (1.02f));
    mBrandLabel.setColour (Label::textColourId, COLOR_GOLD);
    mBrandLabel.setJustificationType (Justification::centred);

    addAndMakeVisible (mTitleLabel);
    addAndMakeVisible (mBrandLabel);
    addAndMakeVisible (mKeyboardViewComponent);
    addChildComponent (mPresetViewComponent);

    addChildComponent (mMenuComponent);
    addChildComponent (mRightClickComponent);

    reconstruct();
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& inGraphics)
{
    auto area = getLocalBounds();
    Styles::fillEditorBackground (inGraphics, area);

    auto brandPanel = Styles::getRelativeBounds (area, SPACE - 4, HEADER_Y - 6, 222, 62).toFloat();
    auto brandGlow = brandPanel.expanded (10.f, 7.f).translated (0.f, -1.f);
    inGraphics.setColour (COLOR_BLUE_GLOW);
    inGraphics.fillRoundedRectangle (brandGlow, 28.f);
    Styles::drawPanel (inGraphics, brandPanel, 24.f, COLOR_PANEL_RAISED.brighter (0.03f), 0.7f);

    auto sideAccent = Rectangle<float> (brandPanel.getX() + 14.f, brandPanel.getY() + 12.f, 4.f, brandPanel.getHeight() - 24.f);
    inGraphics.setColour (COLOR_GOLD.withAlpha (0.55f));
    inGraphics.fillRoundedRectangle (sideAccent, 2.f);

    auto badgeBounds = Rectangle<float> (brandPanel.getX() + 28.f, brandPanel.getY() + 12.f, 94.f, 18.f);
    Styles::drawPill (inGraphics, badgeBounds, COLOR_GOLD.withAlpha (0.12f), COLOR_GOLD.withAlpha (0.52f));

    auto accentBounds = Rectangle<float> (brandPanel.getX() + 28.f, brandPanel.getBottom() - 10.f,
                                          brandPanel.getWidth() - 46.f, 2.f);
    inGraphics.setColour (COLOR_BLUE.withAlpha (0.24f));
    inGraphics.fillRoundedRectangle (accentBounds, 1.f);

    inGraphics.setColour (COLOR_GOLD.withAlpha (0.6f));
    inGraphics.fillEllipse (accentBounds.getRight() - 6.f, accentBounds.getY() - 1.f, 4.f, 4.f);
}

void MainComponent::resized()
{
    auto mainArea = getLocalBounds();
    mKeyboardViewComponent.setBounds (mainArea);
    mPresetViewComponent.setBounds (mainArea);
    mRightClickComponent.setBounds (mainArea);
    mMenuComponent.setBounds (mainArea);

    auto brandPanel = Styles::getRelativeBounds (mainArea, SPACE - 4, HEADER_Y - 6, 222, 62);
    auto brandArea = Rectangle<int> (brandPanel.getX() + 28, brandPanel.getY() + 13, 94, 16);
    auto titleArea = Rectangle<int> (brandPanel.getX() + 28, brandPanel.getY() + 28, brandPanel.getWidth() - 40, 24);

    mTitleLabel.setFont (Styles::getDisplayFont ((float) titleArea.getHeight() + 1.f, true, 0.005f));
    mBrandLabel.setFont (Styles::getAccentFont ((float) brandArea.getHeight() - 2.f, true, 0.08f));
    mTitleLabel.setBounds (titleArea);
    mBrandLabel.setBounds (brandArea);
}

//==============================================================================
void MainComponent::handleNewMessage (const DataMessage* inMessage)
{
    switch (inMessage->messageCode)
    {
        case (MessageCode::kToggleView): { handleToggleView (inMessage); } break;
        case (MessageCode::kToggleMenu): { handleToggleMenu (inMessage); } break;
        case (MessageCode::kToggleRight): { handleToggleRightClick (inMessage); } break;
        case (MessageCode::kPresetFileNew): { handlePresetFileNew (inMessage); } break;
        case (MessageCode::kPresetFileLoaded): { handlePresetFileLoaded (inMessage); } break;
        default: { } break;
    };
}

void MainComponent::handleToggleView (const DataMessage* inMessage)
{
    mKeyboardViewComponent.setVisible (!mGlobalState.isPresetView());
    mPresetViewComponent.setVisible (mGlobalState.isPresetView());
}

void MainComponent::handleToggleMenu (const DataMessage* inMessage)
{
    mMenuComponent.setVisible (mGlobalState.isMenuVisible());
}

void MainComponent::handleToggleRightClick (const DataMessage* inMessage)
{
    mRightClickComponent.setVisible (mGlobalState.isRightClickOn());
}

void MainComponent::handlePresetFileNew (const DataMessage* inMessage)
{
    if (mGlobalState.isPlayMode()) { mGlobalState.toggleMode(); }
    if (mGlobalState.isPresetView()) { mGlobalState.toggleView(); }
}

void MainComponent::handlePresetFileLoaded (const DataMessage* inMessage)
{
    if (mGlobalState.isEditMode()) { mGlobalState.toggleMode(); }
    if (mGlobalState.isPresetView()) { mGlobalState.toggleView(); }
}

void MainComponent::reconstruct()
{
    if (mGlobalState.isPlayMode()) { mGlobalState.toggleMode(); }
    if (mGlobalState.isMenuVisible()) { mGlobalState.toggleMenu(); }
    if (mGlobalState.isPresetView()) { mGlobalState.toggleView(); }
    if (mGlobalState.isEditMode()) { mGlobalState.toggleMode(); }
}
