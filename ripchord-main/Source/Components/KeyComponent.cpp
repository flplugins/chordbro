#include "KeyComponent.h"

//==============================================================================
KeyComponent::KeyComponent (const int inNoteNumber)
:   mNoteNumber (inNoteNumber)
{
    Colour defaultColor = getDefaultColor();
    mNoteColor = defaultColor;
    mMarkerColor = defaultColor;
}

KeyComponent::~KeyComponent()
{
}

//==============================================================================
void KeyComponent::paint (Graphics& inGraphics)
{
    const float markerHeight = 4.f;
    auto keyArea = getLocalBounds().toFloat().reduced (1.f);
    auto noteArea = keyArea;
    noteArea.removeFromBottom (markerHeight + 1.f);

    const float cornerSize = Keyboard::isBlackKey (mNoteNumber) ? 7.f : 9.f;

    inGraphics.setColour (COLOR_BORDER.withAlpha (0.65f));
    inGraphics.fillRoundedRectangle (keyArea, cornerSize);

    inGraphics.setColour (mNoteColor);
    inGraphics.fillRoundedRectangle (noteArea.reduced (0.6f), cornerSize);

    auto markerArea = keyArea.removeFromBottom (markerHeight);
    inGraphics.setColour (mMarkerColor);
    inGraphics.fillRoundedRectangle (markerArea, markerHeight * 0.5f);
}

//==============================================================================
void KeyComponent::mouseUp (const MouseEvent& inEvent)
{
    if (onMouseUp) { onMouseUp (mNoteNumber); }
}

void KeyComponent::mouseDown (const MouseEvent& inEvent)
{
    if (ModifierKeys::getCurrentModifiers().isPopupMenu())
    {
        if (onRightClick) { onRightClick (mNoteNumber, inEvent); }
    }
    else
    {
        if (onMouseDown) { onMouseDown (mNoteNumber); }
    }
}

//==============================================================================
void KeyComponent::setTheme (bool inIsDarkTheme)
{
    mIsDarkTheme = inIsDarkTheme;
}

void KeyComponent::setNoteColor (Colour inColor)
{
    mNoteColor = inColor;
    repaint();
}

void KeyComponent::setMarkerColor (Colour inColor)
{
    mMarkerColor = inColor;
    repaint();
}

void KeyComponent::setNoteAndMarkerColor (Colour inColor)
{
    mNoteColor = inColor;
    mMarkerColor = inColor;
    repaint();
}

//==============================================================================
const int KeyComponent::getNoteNumber()
{
    return mNoteNumber;
}

Colour KeyComponent::getNoteColor()
{
    return mNoteColor;
}

Colour KeyComponent::getMarkerColor()
{
    return mMarkerColor;
}

Colour KeyComponent::getDefaultColor()
{
    Colour whiteKeyColor = mIsDarkTheme ? COLOR_WHITE.withAlpha (0.86f) : COLOR_WHITE;
    return Keyboard::isBlackKey (mNoteNumber) ? COLOR_BLACK : whiteKeyColor;
}
