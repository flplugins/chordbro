#include "PresetComponent.h"

//==============================================================================
PresetComponent::PresetComponent (Preset inPreset)
:   mPreset (inPreset),
    mDeleteComponent ("preset")
{
    mPresetLabel.addMouseListener (this, false);
    mPresetLabel.setText (mPreset.fileName, dontSendNotification);
    mPresetLabel.setColour (Label::textColourId, COLOR_WHITE);
    mPresetLabel.setJustificationType (Justification::centredLeft);

    mImages.setDrawableButtonImages (mStarButton, mPreset.isFavorite ? "StarON.svg" : "Star.svg");
    mImages.setDrawableButtonImages (mTrashButton, "Trash.svg");

    mStarButton.setTriggeredOnMouseDown (true);
    mTrashButton.setTriggeredOnMouseDown (true);

    mTrashButton.onClick = [this]() { mDeleteComponent.setVisible (true); };
    mStarButton.onClick = [this]() { if (onFavorite) { onFavorite (mPreset.indexValue); } };
    mDeleteComponent.onClick = [this]() { if (onDelete) { onDelete (mPreset.indexValue); } };

    addAndMakeVisible (mPresetLabel);
    addAndMakeVisible (mStarButton);
    addAndMakeVisible (mTrashButton);
    addChildComponent (mDeleteComponent);
}

PresetComponent::~PresetComponent()
{
}

//==============================================================================
void PresetComponent::paint (Graphics& inGraphics)
{
    auto area = getLocalBounds().toFloat();
    const float cornerSize = 18.f;
    const Colour fill = mPreset.isFavorite ? COLOR_PANEL_RAISED.brighter (0.1f) : COLOR_PANEL.brighter (0.03f);
    const Colour accent = mPreset.isFavorite ? COLOR_GOLD : COLOR_BLUE;

    Styles::drawPanel (inGraphics, area, cornerSize, fill);

    auto accentBar = Rectangle<float> (area.getX() + 18.f, area.getY() + 12.f, 28.f, 3.f);
    inGraphics.setColour (accent.withAlpha (0.42f));
    inGraphics.fillRoundedRectangle (accentBar, 1.5f);

    if (mPreset.isFavorite)
    {
        inGraphics.setColour (COLOR_GOLD_GLOW);
        inGraphics.fillRoundedRectangle (area.reduced (6.f, 8.f), cornerSize - 4.f);
    }
}

void PresetComponent::resized()
{
    auto area = getLocalBounds();
    mDeleteComponent.setBounds (area);

    juce::Rectangle<float> starAreaProportion (STAR_X / PRESET_WIDTH, STAR_Y / ITEM_HEIGHT,
                                               STAR_WIDTH / PRESET_WIDTH, STAR_HEIGHT / ITEM_HEIGHT);

    juce::Rectangle<float> trashAreaProportion (TRASH_PRESET_X / PRESET_WIDTH, TRASH_PRESET_Y / ITEM_HEIGHT,
                                                TRASH_WIDTH / PRESET_WIDTH, TRASH_HEIGHT / ITEM_HEIGHT);

    mStarButton.setBounds (area.getProportion (starAreaProportion));
    mTrashButton.setBounds (area.getProportion (trashAreaProportion));

    mPresetLabel.setFont (Styles::getUiFont ((area.getHeight() * TEXT_INPUT_FONT_HEIGHT_RATIO) - 1.f, true, 0.012f));
    mPresetLabel.setBounds (area.reduced (area.getHeight() - 4, 0));
}

void PresetComponent::mouseDown (const MouseEvent& inEvent)
{
    if (onClick) { onClick (mPreset.indexValue); }
}
