#include "PresetViewComponent.h"

namespace
{
    Rectangle<int> getPresetBrowserPanelBounds (Rectangle<int> inBounds)
    {
        return Styles::getRelativeBounds (inBounds, PRESET_BROWSER_X, OUTPUT_KEYBOARD_BG_Y,
                                          PRESET_BROWSER_WIDTH, FOOTER_Y - OUTPUT_KEYBOARD_BG_Y - SPACE);
    }
}

//==============================================================================
PresetViewComponent::PresetViewComponent (MainProcess& inMainProcess)
:   mMainProcess (inMainProcess),
    mGlobalState (mMainProcess.getGlobalState()),
    mBrowserState (mMainProcess.getBrowserState()),
    mTagManager (inMainProcess),
    mPresetBrowser (inMainProcess)
{
    mGlobalState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);
    mBrowserState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);

    setWantsKeyboardFocus (true);

    mImages.setDrawableButtonImages (mMenuButton, "GearCircle.svg");
    mImages.setDrawableButtonImages (mPowerButton, mGlobalState.isPowerOn() ? "PowerON.svg" : "Power.svg");
    mImages.setDrawableButtonImages (mFavoritesButton, mBrowserState.isFavoritesOn() ? "FavoritesON.svg" : "Favorites.svg");
    mImages.setDrawableButtonImages (mKeyboardsButton, "Keyboards.svg");

    mMenuButton.setTriggeredOnMouseDown (true);
    mMenuButton.onClick = [this]() { mGlobalState.toggleMenu(); };

    mFavoritesButton.setTriggeredOnMouseDown (true);
    mFavoritesButton.onClick = [this]() { mBrowserState.toggleFavorites(); };

    mKeyboardsButton.setTriggeredOnMouseDown (true);
    mKeyboardsButton.onClick = [this]() { mGlobalState.toggleView(); };

    mPresetViewport.setScrollBarsShown (true, false);
    mPresetViewport.setViewedComponent (&mPresetBrowser, false);

    mPresetFilterInput.setWantsKeyboardFocus (true);
    mPresetFilterInput.setColour (TextEditor::backgroundColourId, COLOR_TRANSPARENT);
    mPresetFilterInput.setColour (TextEditor::textColourId, COLOR_WHITE);
    mPresetFilterInput.setColour (TextEditor::highlightColourId, COLOR_BLUE.withAlpha (0.35f));
    mPresetFilterInput.setColour (TextEditor::highlightedTextColourId, COLOR_WHITE);
    mPresetFilterInput.setColour (CaretComponent::caretColourId, COLOR_GOLD);
    mPresetFilterInput.setTextToShowWhenEmpty ("Search presets", COLOR_GREY.brighter (0.08f));
    mPresetFilterInput.onReturnKey = [this]() { grabKeyboardFocus(); };

    mPresetFilterInput.onTextChange = [this]()
    {
        mBrowserState.handlePresetFilterTextChanged (mPresetFilterInput.getText());
    };

    addAndMakeVisible (mMenuButton);
    addAndMakeVisible (mPowerButton);
    addAndMakeVisible (mFavoritesButton);
    addAndMakeVisible (mKeyboardsButton);
    addAndMakeVisible (mPresetViewport);
    addAndMakeVisible (mPresetFilterInput);
    addChildComponent (mTagManager);
}

PresetViewComponent::~PresetViewComponent()
{
}

//==============================================================================
void PresetViewComponent::paint (Graphics& inGraphics)
{
    auto mainArea = getLocalBounds();
    auto searchPanel = Styles::getRelativeBounds (mainArea, TEXT_INPUT_X, FOOTER_Y, TEXT_INPUT_WIDTH, ITEM_HEIGHT);
    auto browserPanel = getPresetBrowserPanelBounds (mainArea);
    auto actionRail = mMenuButton.getBounds().getUnion (mPowerButton.getBounds()).expanded (12, 8).toFloat();

    Styles::drawInsetField (inGraphics, searchPanel.toFloat(), 17.f, COLOR_BLUE, 0.22f);
    Styles::drawPanel (inGraphics, browserPanel.toFloat(), 24.f, COLOR_PANEL_RAISED.brighter (0.02f), 0.74f);
    Styles::drawPanel (inGraphics, actionRail, actionRail.getHeight() * 0.5f, COLOR_PANEL_RAISED.withAlpha (0.96f), 0.54f);

    auto accent = browserPanel.toFloat();
    accent.setHeight (4.f);
    inGraphics.setColour (COLOR_BLUE.withAlpha (0.18f));
    inGraphics.fillRoundedRectangle (accent, 22.f);
}

void PresetViewComponent::resized()
{
    auto mainArea = getLocalBounds();
    const float topButtonGap = HALF_SPACE + 6.f;
    const float topPowerX = POWER_BUTTON_X;
    const float topMenuX = topPowerX - ITEM_HEIGHT - topButtonGap;
    auto browserPanel = getPresetBrowserPanelBounds (mainArea);

    mTagManager.setBounds (mainArea);
    mMenuButton.setBounds (Styles::getRelativeBounds (mainArea, topMenuX, HEADER_Y, ITEM_HEIGHT, ITEM_HEIGHT));
    mPowerButton.setBounds (Styles::getRelativeBounds (mainArea, topPowerX, HEADER_Y, ITEM_HEIGHT, ITEM_HEIGHT));
    mFavoritesButton.setBounds (Styles::getRelativeBounds (mainArea, LEFT_BUTTON_X, FOOTER_Y, BUTTON_WIDTH, ITEM_HEIGHT));
    mKeyboardsButton.setBounds (Styles::getRelativeBounds (mainArea, RIGHT_BUTTON_X, FOOTER_Y, BUTTON_WIDTH, ITEM_HEIGHT));
    mPresetFilterInput.setBounds (Styles::getRelativeBounds (mainArea, TEXT_INPUT_X, FOOTER_Y, TEXT_INPUT_WIDTH, ITEM_HEIGHT));

    int inputHeight = mPresetFilterInput.getHeight();
    float inputFontHeight = inputHeight * TEXT_INPUT_FONT_HEIGHT_RATIO;

    mPresetFilterInput.applyFontToAllText (Styles::getUiFont (inputFontHeight, true, 0.015f));
    mPresetFilterInput.setIndents ((inputHeight * 0.4f), int ((inputHeight - inputFontHeight) * 0.5f));

    mPresetViewport.setScrollBarThickness (inputHeight * 0.4f);
    mPresetViewport.setBounds (browserPanel.reduced (1, 1));

    auto presetBrowserArea = browserPanel.reduced (1, 1);
    presetBrowserArea.removeFromBottom (1);
    mPresetBrowser.setBounds (presetBrowserArea);
    mPresetBrowser.setDimensions (presetBrowserArea.getWidth(), presetBrowserArea.getHeight());
}

//==============================================================================
void PresetViewComponent::handleNewMessage (const DataMessage* inMessage)
{
    switch (inMessage->messageCode)
    {
        case (MessageCode::kToggleView): { handleToggleView (inMessage); } break;
        case (MessageCode::kToggleFavorites): { handleToggleFavorites (inMessage); } break;
        case (MessageCode::kToggleTagManager): { handleToggleTagManager (inMessage); } break;
        case (MessageCode::kPresetFilterTextChanged): { handlePresetFilterTextChanged (inMessage); } break;
        default: { } break;
    };
}

//==============================================================================
void PresetViewComponent::handleToggleView (const DataMessage* inMessage)
{
    if (mGlobalState.isPresetView() && mBrowserState.isTagSelectorOn()) { mBrowserState.toggleTagSelector(); }
    if (mGlobalState.isPresetView() && mBrowserState.isTagManagerVisible()) { mBrowserState.toggleTagManager(); }

    mBrowserState.refreshData();
    mImages.setDrawableButtonImages (mPowerButton, mGlobalState.isPowerOn() ? "PowerON.svg" : "Power.svg");
}

void PresetViewComponent::handleToggleFavorites (const DataMessage* inMessage)
{
    mImages.setDrawableButtonImages (mFavoritesButton, mBrowserState.isFavoritesOn() ? "FavoritesON.svg" : "Favorites.svg");
}

void PresetViewComponent::handleToggleTagManager (const DataMessage* inMessage)
{
    mTagManager.setVisible (mBrowserState.isTagManagerVisible());
}

void PresetViewComponent::handlePresetFilterTextChanged (const DataMessage* inMessage)
{
    String filterText = inMessage->messageVar1;
    mPresetFilterInput.setText (filterText, dontSendNotification);
}
