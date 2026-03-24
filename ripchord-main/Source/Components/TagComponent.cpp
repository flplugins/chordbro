#include "TagComponent.h"

//==============================================================================
TagComponent::TagComponent (String inName, String inType, bool inIsSelected, bool inIsAssignable)
:   mName (inName),
    mType (inType),
    mIsSelected (inIsSelected),
    mIsAssignable (inIsAssignable),
    mDeleteComponent ("tag")
{
    mTagLabel.addMouseListener (this, false);
    mTagLabel.setText (mName, dontSendNotification);
    mTagLabel.setColour (Label::textColourId, COLOR_WHITE);
    mTagLabel.setJustificationType (Justification::centred);

    addAndMakeVisible (mTagLabel);

    if (mType == "browser")
    {
        mImages.setDrawableButtonImages (mUpButton, "Up.svg");
        mImages.setDrawableButtonImages (mDownButton, "Down.svg");
        mImages.setDrawableButtonImages (mTrashButton, "Trash.svg");

        mUpButton.setTriggeredOnMouseDown (true);
        mDownButton.setTriggeredOnMouseDown (true);
        mTrashButton.setTriggeredOnMouseDown (true);

        mUpButton.onClick = [this]() { if (onUpShift) { onUpShift (mName); } };
        mDownButton.onClick = [this]() { if (onDownShift) { onDownShift (mName); } };
        mTrashButton.onClick = [this]() { mDeleteComponent.setVisible (true); };
        mDeleteComponent.onClick = [this]() { if (onDelete) { onDelete (mName); } };

        addAndMakeVisible (mUpButton);
        addAndMakeVisible (mDownButton);
        addAndMakeVisible (mTrashButton);
        addChildComponent (mDeleteComponent);
    }
}

TagComponent::~TagComponent()
{
}

//==============================================================================
void TagComponent::paint (Graphics& inGraphics)
{
    const Colour fill = mIsSelected
        ? COLOR_BLUE.withAlpha (0.22f)
        : mIsAssignable
            ? COLOR_GREEN.withAlpha (0.18f)
            : COLOR_PANEL_SOFT;

    const Colour outline = mIsSelected
        ? COLOR_BLUE
        : mIsAssignable
            ? COLOR_GREEN
            : COLOR_BORDER;

    Styles::drawPill (inGraphics, getLocalBounds().toFloat(), fill, outline);
}

void TagComponent::resized()
{
    auto area = getLocalBounds();
    mTagLabel.setFont (Font ((area.getHeight() * TEXT_INPUT_FONT_HEIGHT_RATIO) + 1).boldened());

    if (mType == "bar")
    {
        mTagLabel.setBounds (area.reduced ((area.getHeight() * 0.1f), 0));
    }

    if (mType == "browser")
    {
        mDeleteComponent.setBounds (area);
        mTagLabel.setBounds (area.reduced ((area.getHeight() * 0.7f), 0));

        juce::Rectangle<float> upAreaProportion (SHIFT_TAG_X / PRESET_WIDTH, SHIFT_UP_TAG_Y / ITEM_HEIGHT,
                                                 SHIFT_TAG_WIDTH / PRESET_WIDTH, SHIFT_TAG_HEIGHT / ITEM_HEIGHT);

        juce::Rectangle<float> downAreaProportion (SHIFT_TAG_X / PRESET_WIDTH, SHIFT_DOWN_TAG_Y / ITEM_HEIGHT,
                                                   SHIFT_TAG_WIDTH / PRESET_WIDTH, SHIFT_TAG_HEIGHT / ITEM_HEIGHT);

        juce::Rectangle<float> trashAreaProportion (TRASH_TAG_X / PRESET_WIDTH, TRASH_TAG_Y / ITEM_HEIGHT,
                                                    TRASH_WIDTH / PRESET_WIDTH, TRASH_HEIGHT / ITEM_HEIGHT);

        mUpButton.setBounds (area.getProportion (upAreaProportion));
        mDownButton.setBounds (area.getProportion (downAreaProportion));
        mTrashButton.setBounds (area.getProportion (trashAreaProportion));
    }
}

//==============================================================================
void TagComponent::mouseDown (const MouseEvent& inEvent)
{
    if (mType == "bar" && onClick) { onClick (mName); }
}
