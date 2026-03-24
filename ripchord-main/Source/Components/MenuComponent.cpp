#include "MenuComponent.h"

namespace
{
    class PremiumTextButtonLookAndFeel : public LookAndFeel_V4
    {
    public:
        Font getTextButtonFont (TextButton&, int inButtonHeight) override
        {
            return Styles::getUiFont ((float) inButtonHeight * 0.48f, true, 0.018f);
        }

        void drawButtonBackground (Graphics& inGraphics,
                                   Button& inButton,
                                   const Colour& inBackgroundColour,
                                   bool inIsMouseOverButton,
                                   bool inIsButtonDown) override
        {
            auto bounds = inButton.getLocalBounds().toFloat().reduced (0.5f);
            auto fillColour = inBackgroundColour;
            auto outlineColour = inButton.findColour (TextButton::buttonOnColourId);

            if (!inButton.isEnabled())
            {
                fillColour = fillColour.withMultipliedAlpha (0.45f);
                outlineColour = outlineColour.withMultipliedAlpha (0.35f);
            }
            else if (inIsButtonDown)
            {
                fillColour = fillColour.brighter (0.18f);
            }
            else if (inIsMouseOverButton)
            {
                fillColour = fillColour.brighter (0.08f);
            }

            auto shadowBounds = bounds.translated (0.f, 3.f).expanded (-2.f, 0.f);
            inGraphics.setColour (Colour (0, 0, 0).withAlpha (0.2f));
            inGraphics.fillRoundedRectangle (shadowBounds, 10.f);

            ColourGradient fill (fillColour.brighter (0.14f), bounds.getX(), bounds.getY(),
                                 fillColour.darker (0.1f), bounds.getCentreX(), bounds.getBottom(), false);
            inGraphics.setGradientFill (fill);
            inGraphics.fillRoundedRectangle (bounds, 10.f);

            auto highlight = Rectangle<float> (bounds.getX() + 12.f, bounds.getY() + 9.f, bounds.getWidth() - 24.f, 2.f);
            inGraphics.setColour (COLOR_WHITE.withAlpha (0.07f));
            inGraphics.fillRoundedRectangle (highlight, 1.f);

            inGraphics.setColour (outlineColour.withAlpha (0.78f));
            inGraphics.drawRoundedRectangle (bounds, 10.f, 1.f);
        }

        void drawButtonText (Graphics& inGraphics,
                             TextButton& inButton,
                             bool,
                             bool) override
        {
            auto bounds = inButton.getLocalBounds().reduced (12, 0);
            auto textColour = inButton.findColour (TextButton::textColourOffId);

            if (!inButton.isEnabled()) { textColour = textColour.withMultipliedAlpha (0.45f); }

            inGraphics.setColour (textColour);
            inGraphics.setFont (getTextButtonFont (inButton, inButton.getHeight()));
            inGraphics.drawFittedText (inButton.getButtonText(), bounds, Justification::centred, 1);
        }
    };

    PremiumTextButtonLookAndFeel& getPremiumButtonLookAndFeel()
    {
        static PremiumTextButtonLookAndFeel lookAndFeel;
        return lookAndFeel;
    }

    Rectangle<int> getMenuModalBounds (Rectangle<int> inBounds)
    {
        return Styles::getRelativeBounds (inBounds, MODAL_X, MODAL_Y, MODAL_WIDTH, MODAL_HEIGHT);
    }

    void stylePremiumButton (TextButton& inButton, Colour inFill, Colour inOutline, Colour inTextColour = COLOR_WHITE)
    {
        inButton.setLookAndFeel (&getPremiumButtonLookAndFeel());
        inButton.setColour (TextButton::buttonColourId, inFill);
        inButton.setColour (TextButton::buttonOnColourId, inOutline);
        inButton.setColour (TextButton::textColourOffId, inTextColour);
        inButton.setColour (TextButton::textColourOnId, inTextColour);
        inButton.setTriggeredOnMouseDown (true);
        inButton.setMouseCursor (MouseCursor::PointingHandCursor);
    }
}

//==============================================================================
MenuComponent::MenuComponent (MainProcess& inMainProcess)
:   mMainProcess (inMainProcess),
    mGlobalState (mMainProcess.getGlobalState()),
    mPresetState (mMainProcess.getPresetState())
{
    mGlobalState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);

    mUtilityLabel.setColour (Label::textColourId, COLOR_GOLD);
    mActionsLabel.setColour (Label::textColourId, COLOR_BLUE.brighter (0.2f));
    mUtilityLabel.setJustificationType (Justification::centredLeft);
    mActionsLabel.setJustificationType (Justification::centredLeft);
    mCaptionLabel.setColour (Label::textColourId, COLOR_GREY.brighter (0.15f));
    mCaptionLabel.setJustificationType (Justification::centredLeft);

    stylePremiumButton (mHelpButton, COLOR_BLUE.withAlpha (0.16f), COLOR_BLUE.withAlpha (0.56f));
    stylePremiumButton (mResetUtilityButton, COLOR_PANEL_RAISED.withAlpha (0.96f), COLOR_BORDER.withAlpha (0.62f));
    stylePremiumButton (mImportPresetButton, COLOR_PANEL_RAISED.brighter (0.02f), COLOR_BLUE.withAlpha (0.52f));
    stylePremiumButton (mExportMidiButton, COLOR_GOLD.withAlpha (0.14f), COLOR_GOLD.withAlpha (0.7f));

    mHelpButton.onClick = [this]()
    {
        URL url { "https://chordbro.com" };
        url.launchInDefaultBrowser();
        mGlobalState.toggleMenu();
    };

    mResetUtilityButton.onClick = [this]()
    {
        mGlobalState.resetSizeInConfig();
    };

    mExportMidiButton.onClick = [this]()
    {
        if (!mPresetState.isPresetValid()) { return; }
        mPresetState.handleClickExportMidi();
        mGlobalState.toggleMenu();
    };

    mImportPresetButton.onClick = [this]()
    {
        mPresetState.handleClickImportPreset();
        mGlobalState.toggleMenu();
    };

    addAndMakeVisible (mUtilityLabel);
    addAndMakeVisible (mActionsLabel);
    addAndMakeVisible (mCaptionLabel);
    addAndMakeVisible (mHelpButton);
    addAndMakeVisible (mResetUtilityButton);
    addAndMakeVisible (mExportMidiButton);
    addAndMakeVisible (mImportPresetButton);
}

MenuComponent::~MenuComponent()
{
    mHelpButton.setLookAndFeel (nullptr);
    mResetUtilityButton.setLookAndFeel (nullptr);
    mImportPresetButton.setLookAndFeel (nullptr);
    mExportMidiButton.setLookAndFeel (nullptr);
}

//==============================================================================
void MenuComponent::paint (Graphics& inGraphics)
{
    inGraphics.setColour (COLOR_OVERLAY);
    inGraphics.fillRect (getLocalBounds());

    auto mainArea = getLocalBounds();
    auto modalArea = getMenuModalBounds (mainArea).toFloat();
    Styles::drawPanel (inGraphics, modalArea, 26.f, COLOR_PANEL_RAISED.brighter (0.03f), 0.72f);

    auto accent = modalArea;
    accent.setHeight (4.f);
    inGraphics.setColour (COLOR_BLUE.withAlpha (0.24f));
    inGraphics.fillRoundedRectangle (accent, 24.f);

    auto contentArea = getMenuModalBounds (mainArea).reduced (24, 22);
    auto utilityArea = contentArea.removeFromLeft (168).toFloat();
    contentArea.removeFromLeft (24);
    auto actionsArea = contentArea.removeFromRight (232).toFloat();

    Styles::drawPanel (inGraphics, utilityArea, 18.f, COLOR_PANEL_SOFT.brighter (0.08f), 0.68f);
    Styles::drawPanel (inGraphics, actionsArea, 18.f, COLOR_PANEL_SOFT.brighter (0.06f), 0.66f);

    auto utilityAccent = Rectangle<float> (utilityArea.getX() + 14.f, utilityArea.getY() + 14.f, utilityArea.getWidth() - 28.f, 2.f);
    inGraphics.setColour (COLOR_GOLD.withAlpha (0.28f));
    inGraphics.fillRoundedRectangle (utilityAccent, 1.f);

    auto actionsAccent = Rectangle<float> (actionsArea.getX() + 14.f, actionsArea.getY() + 14.f, actionsArea.getWidth() - 28.f, 2.f);
    inGraphics.setColour (COLOR_BLUE.withAlpha (0.26f));
    inGraphics.fillRoundedRectangle (actionsAccent, 1.f);
}

void MenuComponent::resized()
{
    auto modalArea = getMenuModalBounds (getLocalBounds()).reduced (24, 22);
    auto utilityArea = modalArea.removeFromLeft (168);
    modalArea.removeFromLeft (24);

    auto actionsArea = Rectangle<int> (modalArea.getRight() - 232, modalArea.getY(), 232, modalArea.getHeight());

    auto utilityHeader = utilityArea.removeFromTop (18);
    utilityArea.removeFromTop (16);
    auto actionsHeader = actionsArea.removeFromTop (18);
    actionsArea.removeFromTop (16);

    const int utilityButtonHeight = roundToInt (ITEM_HEIGHT) + 2;
    auto helpArea = utilityArea.removeFromTop (utilityButtonHeight);
    utilityArea.removeFromTop (12);
    auto resetArea = utilityArea.removeFromTop (utilityButtonHeight);
    auto captionArea = Rectangle<int> (utilityArea.getX(), getMenuModalBounds (getLocalBounds()).getBottom() - 52,
                                       utilityArea.getWidth(), 18);

    mUtilityLabel.setFont (Styles::getAccentFont ((float) utilityHeader.getHeight() - 1.f, true, 0.08f));
    mActionsLabel.setFont (Styles::getAccentFont ((float) actionsHeader.getHeight() - 1.f, true, 0.08f));
    mCaptionLabel.setFont (Styles::getUiFont (13.f, false, 0.012f));
    mUtilityLabel.setBounds (utilityHeader);
    mActionsLabel.setBounds (actionsHeader);
    mCaptionLabel.setBounds (captionArea);
    mHelpButton.setBounds (helpArea);
    mResetUtilityButton.setBounds (resetArea);

    const int stackGap = 14;
    const int actionButtonHeight = roundToInt (ITEM_HEIGHT) + 8;
    const int stackHeight = (actionButtonHeight * 2) + stackGap;
    const int stackTop = actionsArea.getY() + ((actionsArea.getHeight() - stackHeight) / 2);

    mImportPresetButton.setBounds (actionsArea.getX(), stackTop, actionsArea.getWidth(), actionButtonHeight);
    mExportMidiButton.setBounds (actionsArea.getX(), stackTop + actionButtonHeight + stackGap, actionsArea.getWidth(), actionButtonHeight);
}

//==============================================================================
void MenuComponent::mouseDown (const MouseEvent& inEvent)
{
    mGlobalState.toggleMenu();
}

//==============================================================================
void MenuComponent::handleNewMessage (const DataMessage* inMessage)
{
    switch (inMessage->messageCode)
    {
        case (MessageCode::kToggleMenu): { handleToggleMenu (inMessage); } break;
        default: { } break;
    };
}

void MenuComponent::handleToggleMenu (const DataMessage* inMessage)
{
    if (mGlobalState.isMenuHidden()) { return; }

    mExportMidiButton.setEnabled (mPresetState.isPresetValid());
}
