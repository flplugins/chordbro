#pragma once

#include "JuceHeader.h"

using namespace juce;

const Colour COLOR_THEME_DARK = Colour (7, 10, 16);
const Colour COLOR_THEME_LIGHT = Colour (226, 230, 236);

const Colour COLOR_BLACK = Colour (11, 15, 23);
const Colour COLOR_DARK = Colour (25, 31, 43);
const Colour COLOR_GREY = Colour (136, 145, 163);
const Colour COLOR_WHITE = Colour (241, 245, 250);

const Colour COLOR_RED = Colour (214, 92, 108);
const Colour COLOR_BLUE = Colour (91, 174, 245);
const Colour COLOR_GREEN = Colour (93, 194, 170);
const Colour COLOR_PURPLE = Colour (108, 118, 196);
const Colour COLOR_BLUE_DARK = Colour (44, 94, 175);
const Colour COLOR_GOLD = Colour (214, 182, 118);

const Colour COLOR_PANEL = Colour (17, 22, 32);
const Colour COLOR_PANEL_RAISED = Colour (26, 33, 46);
const Colour COLOR_PANEL_SOFT = Colour (12, 16, 24);
const Colour COLOR_BORDER = Colour (72, 86, 110);
const Colour COLOR_OVERLAY = Colour (2, 4, 8).withAlpha (0.82f);
const Colour COLOR_TRANSPARENT = Colour (0, 0, 0).withAlpha (0.0f);
const Colour COLOR_BLUE_GLOW = Colour (91, 174, 245).withAlpha (0.12f);
const Colour COLOR_GOLD_GLOW = Colour (214, 182, 118).withAlpha (0.10f);

const float EDITOR_WIDTH = 1000.f;
const float SPACE = 20.f;
const float DOUBLE_SPACE = SPACE * 2;
const float HALF_SPACE = SPACE / 2;
const float HEADER_Y = SPACE;

const float ITEM_HEIGHT = 34.f;
const float BUTTON_WIDTH = 120.f;
const float CORNER_SIZE_RATIO = 0.15f;
const float TEXT_INPUT_FONT_HEIGHT_RATIO = 0.6f;

const float SLIDER_Y = 1.f;
const float SLIDER_SIZE = 72.f;
const float SLIDER_X_OFFSET = 19.f;

const float KEYBOARD_BG_HEIGHT = 120.f;
const float KEYBOARD_BG_WIDTH = EDITOR_WIDTH - DOUBLE_SPACE;
const float OUTPUT_KEYBOARD_BG_Y = ITEM_HEIGHT + DOUBLE_SPACE;
const float INPUT_KEYBOARD_BG_Y = OUTPUT_KEYBOARD_BG_Y + KEYBOARD_BG_HEIGHT + DOUBLE_SPACE + ITEM_HEIGHT;

const float CONTROLS_HEIGHT = DOUBLE_SPACE + ITEM_HEIGHT;
const float CONTROLS_WIDTH = KEYBOARD_BG_WIDTH;
const float CONTROLS_Y = OUTPUT_KEYBOARD_BG_Y + KEYBOARD_BG_HEIGHT;
const float CONTROLS_OUTER_SPACE = SPACE * 7.f;

const float RECORD_BUTTON_X = 0;

const float VELOCITY_DIRECTION_X = CONTROLS_OUTER_SPACE;
const float VELOCITY_DEPTH_X = VELOCITY_DIRECTION_X + ITEM_HEIGHT + SPACE;
const float VELOCITY_VARIANCE_X = VELOCITY_DEPTH_X + ITEM_HEIGHT + SPACE + 9.f;

const float TRANSPOSE_BUTTON_X = (KEYBOARD_BG_WIDTH * 0.5f) - (ITEM_HEIGHT * 0.5f);
const float SHIFT_LEFT_BUTTON_X = TRANSPOSE_BUTTON_X - ITEM_HEIGHT - SPACE;
const float SHIFT_RIGHT_BUTTON_X = TRANSPOSE_BUTTON_X + ITEM_HEIGHT + SPACE;

const float SAVE_BUTTON_X = TRANSPOSE_BUTTON_X;
const float EDIT_LEFT_BUTTON_X = SHIFT_LEFT_BUTTON_X;
const float EDIT_RIGHT_BUTTON_X = SHIFT_RIGHT_BUTTON_X;
const float ALL_WHITE_BUTTON_X = EDIT_LEFT_BUTTON_X - ITEM_HEIGHT - SPACE;
const float ALL_BLACK_BUTTON_X = EDIT_RIGHT_BUTTON_X + ITEM_HEIGHT + SPACE;

const float DELAY_DIRECTION_X = KEYBOARD_BG_WIDTH - ITEM_HEIGHT - CONTROLS_OUTER_SPACE;
const float DELAY_DEPTH_X = DELAY_DIRECTION_X - ITEM_HEIGHT - SPACE;
const float DELAY_VARIANCE_X = DELAY_DEPTH_X - ITEM_HEIGHT - SPACE - 9.f;

const float RECORDED_BUTTON_X = CONTROLS_WIDTH - ITEM_HEIGHT;

const float KEYBOARD_LABEL_HEIGHT = 18.f;
const float KEYBOARD_LABEL_WIDTH = 200.f;
const float KEYBOARD_LABEL_X = SPACE + HALF_SPACE - 3;
const float OUTPUT_LABEL_Y = OUTPUT_KEYBOARD_BG_Y + HALF_SPACE;
const float INPUT_LABEL_Y = INPUT_KEYBOARD_BG_Y + HALF_SPACE;

const float KEYBOARD_WIDTH = 936.f;
const float KEYBOARD_HEIGHT = KEYBOARD_BG_HEIGHT - KEYBOARD_LABEL_HEIGHT - (3 * HALF_SPACE);
const float KEYBOARD_X = EDITOR_WIDTH * 0.5f - KEYBOARD_WIDTH * 0.5f;
const float OUTPUT_KEYBOARD_Y = OUTPUT_KEYBOARD_BG_Y + KEYBOARD_LABEL_HEIGHT + SPACE;
const float INPUT_KEYBOARD_Y = INPUT_KEYBOARD_BG_Y + KEYBOARD_LABEL_HEIGHT + SPACE;

const float THROUGH_X = EDITOR_WIDTH * 0.5f - ITEM_HEIGHT * 0.5f;
const float THROUGH_Y = OUTPUT_KEYBOARD_BG_Y + KEYBOARD_BG_HEIGHT + SPACE;

const float ARROW_WIDTH = 22.f;
const float TEXT_INPUT_WIDTH = KEYBOARD_BG_WIDTH - (4 * BUTTON_WIDTH);
const float TEXT_INPUT_X = EDITOR_WIDTH * 0.5f - TEXT_INPUT_WIDTH * 0.5f;
const float LEFT_BUTTON_X = SPACE;
const float RIGHT_BUTTON_X = SPACE + KEYBOARD_BG_WIDTH - BUTTON_WIDTH;
const float POWER_BUTTON_X = SPACE + KEYBOARD_BG_WIDTH - ITEM_HEIGHT;
const float MENU_BUTTON_X = POWER_BUTTON_X - ITEM_HEIGHT - HALF_SPACE;
const float FAV_BUTTON_X = MENU_BUTTON_X - ITEM_HEIGHT - HALF_SPACE;

const float PRESET_BROWSER_X = SPACE;
const float PRESET_BROWSER_Y = OUTPUT_KEYBOARD_BG_Y + ITEM_HEIGHT + SPACE;
const float PRESET_BROWSER_WIDTH = KEYBOARD_BG_WIDTH;
const float PRESET_BROWSER_HEIGHT = (2 * KEYBOARD_BG_HEIGHT) + SPACE;

const float PRESET_VIEWPORT_X = PRESET_BROWSER_X + 1;
const float PRESET_VIEWPORT_Y = PRESET_BROWSER_Y + 1;
const float PRESET_VIEWPORT_WIDTH = PRESET_BROWSER_WIDTH + SPACE - 4.5f;
const float PRESET_VIEWPORT_HEIGHT = PRESET_BROWSER_HEIGHT - 2;

const int PRESETS_PER_ROW = 3;
const float PRESET_WIDTH = (PRESET_BROWSER_WIDTH - ((PRESETS_PER_ROW + 1) * HALF_SPACE)) / PRESETS_PER_ROW;
const float PRESET_PAD_X = 8.f;
const float PRESET_PAD_Y = 7.f;

const float TAG_MANAGER_X = SPACE;
const float TAG_SELECTOR_X = TAG_MANAGER_X + ITEM_HEIGHT + HALF_SPACE;

const float BAR_TAG_HEIGHT = 22.f;
const float BAR_TAG_WIDTH = BUTTON_WIDTH;

const float TAG_BAR_X = TAG_SELECTOR_X + ITEM_HEIGHT + HALF_SPACE;
const float TAG_BAR_Y = OUTPUT_KEYBOARD_BG_Y;
const float TAG_BAR_WIDTH = KEYBOARD_BG_WIDTH - SPACE - (2 * ITEM_HEIGHT);
const float TAG_BAR_HEIGHT = ITEM_HEIGHT;

const float TAG_BAR_VIEWPORT_X = TAG_BAR_X + 1;
const float TAG_BAR_VIEWPORT_Y = TAG_BAR_Y + 1;
const float TAG_BAR_VIEWPORT_WIDTH = TAG_BAR_WIDTH - 2;
const float TAG_BAR_VIEWPORT_HEIGHT = TAG_BAR_HEIGHT + SPACE - 4.5f;

const float STAR_WIDTH = 20.f;
const float STAR_HEIGHT = 20.f;
const float STAR_X = PRESET_PAD_X;
const float STAR_Y = PRESET_PAD_Y - 0.5f;

const float SHIFT_TAG_WIDTH = 18.f;
const float SHIFT_TAG_HEIGHT = 8.f;
const float SHIFT_TAG_X = PRESET_PAD_X;
const float SHIFT_UP_TAG_Y = PRESET_PAD_Y + 1;
const float SHIFT_DOWN_TAG_Y = SHIFT_UP_TAG_Y + SHIFT_TAG_HEIGHT + 4;

const float TRASH_WIDTH = 16.f;
const float TRASH_HEIGHT = 20.f;
const float TRASH_PRESET_X = PRESET_WIDTH - TRASH_WIDTH - PRESET_PAD_X;
const float TRASH_PRESET_Y = PRESET_PAD_Y;
const float TRASH_TAG_X = TRASH_PRESET_X;
const float TRASH_TAG_Y = TRASH_PRESET_Y + 1;

const float CHECK_WIDTH = 22.f;
const float CHECK_HEIGHT = 22.f;
const float CHECK_X = PRESET_PAD_X - 1;
const float CHECK_Y = PRESET_PAD_Y - 1;

const float FOOTER_Y = INPUT_KEYBOARD_BG_Y + KEYBOARD_BG_HEIGHT + SPACE;
const float EDITOR_HEIGHT = FOOTER_Y + ITEM_HEIGHT + SPACE;

const float MODAL_WIDTH = 496.f;
const float MODAL_HEIGHT = 310.f;
const float MODAL_X = EDITOR_WIDTH * 0.5f - MODAL_WIDTH * 0.5f;
const float MODAL_Y = EDITOR_HEIGHT * 0.5f - MODAL_HEIGHT * 0.5f;

const float TAG_INPUT_X = MODAL_X + SPACE;
const float TAG_INPUT_Y = MODAL_Y + SPACE;
const float TAG_INPUT_WIDTH = MODAL_WIDTH - BUTTON_WIDTH - (SPACE * 3);
const float CREATE_TAG_X = TAG_INPUT_X + TAG_INPUT_WIDTH + SPACE;

const float TAG_BROWSER_X = TAG_INPUT_X;
const float TAG_BROWSER_Y = TAG_INPUT_Y + ITEM_HEIGHT + SPACE;
const float TAG_BROWSER_WIDTH = MODAL_WIDTH - DOUBLE_SPACE;
const float TAG_BROWSER_HEIGHT = MODAL_HEIGHT - ITEM_HEIGHT - (SPACE * 3);

const float TAG_VIEWPORT_X = TAG_BROWSER_X + 1;
const float TAG_VIEWPORT_Y = TAG_BROWSER_Y + 1;
const float TAG_VIEWPORT_WIDTH = TAG_BROWSER_WIDTH + SPACE - 5;
const float TAG_VIEWPORT_HEIGHT = TAG_BROWSER_HEIGHT - 2;

const int BROWSER_TAGS_PER_ROW = 2;
const float BROWSER_TAG_WIDTH = ((TAG_BROWSER_WIDTH - 1) - ((BROWSER_TAGS_PER_ROW + 1) * HALF_SPACE)) / BROWSER_TAGS_PER_ROW;

const float CANCEL_WIDTH = 80.f;
const float CANCEL_HEIGHT = 24.f;
const float DELETE_WIDTH = CANCEL_WIDTH;
const float DELETE_HEIGHT = CANCEL_HEIGHT;
const float CANCEL_Y = 5.5f;
const float DELETE_Y = CANCEL_Y;

const float CANCEL_PRESET_X = (PRESET_WIDTH / 2) - CANCEL_WIDTH - 25;
const float DELETE_PRESET_X = (PRESET_WIDTH / 2) + 25;
const float CANCEL_TAG_X = (BROWSER_TAG_WIDTH / 2) - CANCEL_WIDTH - 9;
const float DELETE_TAG_X = (BROWSER_TAG_WIDTH / 2) + 9;

const float MENU_ITEM_HEIGHT = 30.f;
const float MENU_ACTION_WIDTH = 220.f;
const float MENU_TOGGLE_WIDTH = 120.f;
const float MENU_ACTION_X = EDITOR_WIDTH * 0.5f + HALF_SPACE;
const float MENU_ACTION_Y_01 = MODAL_Y + SPACE;
const float MENU_ACTION_Y_02 = MENU_ACTION_Y_01 + MENU_ITEM_HEIGHT + HALF_SPACE;
const float MENU_ACTION_Y_03 = MENU_ACTION_Y_02 + MENU_ITEM_HEIGHT + HALF_SPACE;
const float MENU_ACTION_Y_04 = MENU_ACTION_Y_03 + MENU_ITEM_HEIGHT + HALF_SPACE;
const float MENU_ACTION_Y_05 = MENU_ACTION_Y_04 + MENU_ITEM_HEIGHT + HALF_SPACE;
const float MENU_ACTION_Y_06 = MENU_ACTION_Y_05 + MENU_ITEM_HEIGHT + HALF_SPACE;
const float MENU_ACTION_Y_07 = MENU_ACTION_Y_06 + MENU_ITEM_HEIGHT + HALF_SPACE;

const float RIGHT_CLICK_ITEM_HEIGHT = 24.f;
const float RIGHT_CLICK_ITEM_WIDTH = 80.f;
const float RIGHT_CLICK_MENU_HEIGHT = 102.f;
const float RIGHT_CLICK_MENU_WIDTH = 100.f;

//==============================================================================
namespace Styles
{
    static inline Font getDisplayFont (float inHeight, bool inBold = true, float inKerning = 0.0f)
    {
        return Font ("Segoe UI Variable Display", inHeight, inBold ? Font::bold : Font::plain)
            .withExtraKerningFactor (inKerning);
    }

    static inline Font getUiFont (float inHeight, bool inBold = false, float inKerning = 0.0f)
    {
        return Font ("Segoe UI Variable Text", inHeight, inBold ? Font::bold : Font::plain)
            .withExtraKerningFactor (inKerning);
    }

    static inline Font getAccentFont (float inHeight, bool inBold = true, float inKerning = 0.04f)
    {
        return Font ("Bahnschrift", inHeight, inBold ? Font::bold : Font::plain)
            .withExtraKerningFactor (inKerning);
    }

    static inline Rectangle<int> getRelativeBounds (Rectangle<int> inBaselineBounds, int inX, int inY, int inWidth, int inHeight)
    {
        Rectangle<float> proportions { inX / EDITOR_WIDTH, inY / EDITOR_HEIGHT, inWidth / EDITOR_WIDTH, inHeight / EDITOR_HEIGHT };
        return inBaselineBounds.getProportion (proportions);
    }

    static inline void fillEditorBackground (Graphics& inGraphics, Rectangle<int> inBounds)
    {
        const auto area = inBounds.toFloat();
        ColourGradient background (COLOR_THEME_DARK.brighter (0.12f), area.getX(), area.getY(),
                                   COLOR_THEME_DARK.darker (0.48f), area.getCentreX(), area.getBottom(), false);
        background.addColour (0.32, Colour (13, 18, 28));
        background.addColour (0.68, Colour (8, 12, 19));
        inGraphics.setGradientFill (background);
        inGraphics.fillAll();

        inGraphics.setColour (COLOR_BLUE.withAlpha (0.15f));
        inGraphics.fillEllipse (Rectangle<float> (area.getWidth() * 0.46f, -area.getHeight() * 0.18f,
                                                  area.getWidth() * 0.48f, area.getHeight() * 0.48f));

        inGraphics.setColour (COLOR_PURPLE.withAlpha (0.06f));
        inGraphics.fillEllipse (Rectangle<float> (-area.getWidth() * 0.2f, area.getHeight() * 0.08f,
                                                  area.getWidth() * 0.44f, area.getHeight() * 0.44f));

        inGraphics.setColour (COLOR_GOLD.withAlpha (0.05f));
        inGraphics.fillEllipse (Rectangle<float> (area.getWidth() * 0.08f, area.getHeight() * 0.7f,
                                                  area.getWidth() * 0.38f, area.getHeight() * 0.22f));

        auto topRule = Rectangle<float> (area.getX() + 18.f, area.getY() + 14.f, area.getWidth() - 36.f, 1.5f);
        inGraphics.setColour (COLOR_WHITE.withAlpha (0.04f));
        inGraphics.fillRoundedRectangle (topRule, 0.75f);
    }

    static inline void drawPanel (Graphics& inGraphics, Rectangle<float> inBounds, float inCornerSize, Colour inFillColour = COLOR_PANEL, float inOutlineAlpha = 0.8f)
    {
        Path panelPath;
        panelPath.addRoundedRectangle (inBounds, inCornerSize);

        auto shadowBounds = inBounds.translated (0.f, 4.f).expanded (-2.f, 0.f);
        inGraphics.setColour (Colour (0, 0, 0).withAlpha (0.22f));
        inGraphics.fillRoundedRectangle (shadowBounds, jmax (3.f, inCornerSize - 1.f));

        ColourGradient fill (inFillColour.brighter (0.13f), inBounds.getX(), inBounds.getY(),
                             inFillColour.darker (0.11f), inBounds.getCentreX(), inBounds.getBottom(), false);
        fill.addColour (0.4, inFillColour.brighter (0.05f));
        inGraphics.setGradientFill (fill);
        inGraphics.fillPath (panelPath);

        auto topHighlight = Rectangle<float> (inBounds.getX() + 12.f, inBounds.getY() + 10.f,
                                              inBounds.getWidth() - 24.f, 2.f);
        inGraphics.setColour (COLOR_WHITE.withAlpha (0.05f));
        inGraphics.fillRoundedRectangle (topHighlight, 1.f);

        inGraphics.setColour (COLOR_WHITE.withAlpha (0.05f));
        inGraphics.drawRoundedRectangle (inBounds.reduced (0.5f), inCornerSize, 1.f);

        inGraphics.setColour (COLOR_BORDER.withAlpha (inOutlineAlpha));
        inGraphics.drawRoundedRectangle (inBounds.reduced (1.f), jmax (3.f, inCornerSize - 1.f), 1.f);
    }

    static inline void drawPanel (Graphics& inGraphics, Rectangle<int> inBounds, float inCornerSize, Colour inFillColour = COLOR_PANEL, float inOutlineAlpha = 0.8f)
    {
        drawPanel (inGraphics, inBounds.toFloat(), inCornerSize, inFillColour, inOutlineAlpha);
    }

    static inline void drawPill (Graphics& inGraphics, Rectangle<float> inBounds, Colour inFillColour, Colour inOutlineColour)
    {
        const float corner = inBounds.getHeight() * 0.5f;
        ColourGradient fill (inFillColour.brighter (0.18f), inBounds.getX(), inBounds.getY(),
                             inFillColour.darker (0.12f), inBounds.getCentreX(), inBounds.getBottom(), false);
        inGraphics.setGradientFill (fill);
        inGraphics.fillRoundedRectangle (inBounds, corner);
        inGraphics.setColour (inOutlineColour.withAlpha (0.9f));
        inGraphics.drawRoundedRectangle (inBounds.reduced (0.5f), corner, 1.f);
    }

    static inline void drawInsetField (Graphics& inGraphics, Rectangle<float> inBounds, float inCornerSize,
                                       Colour inAccentColour, float inAccentAlpha = 0.22f)
    {
        const auto fillTop = COLOR_PANEL_SOFT.brighter (0.08f);
        const auto fillBottom = COLOR_BLACK.withAlpha (0.96f);

        ColourGradient fill (fillTop, inBounds.getX(), inBounds.getY(),
                             fillBottom, inBounds.getCentreX(), inBounds.getBottom(), false);
        inGraphics.setGradientFill (fill);
        inGraphics.fillRoundedRectangle (inBounds, inCornerSize);

        inGraphics.setColour (COLOR_WHITE.withAlpha (0.04f));
        inGraphics.drawRoundedRectangle (inBounds.reduced (0.5f), inCornerSize, 1.f);

        inGraphics.setColour (COLOR_BORDER.withAlpha (0.55f));
        inGraphics.drawRoundedRectangle (inBounds.reduced (1.f), jmax (3.f, inCornerSize - 1.f), 1.f);

        auto accent = Rectangle<float> (inBounds.getX() + 18.f, inBounds.getBottom() - 4.f,
                                        inBounds.getWidth() - 36.f, 2.f);
        inGraphics.setColour (inAccentColour.withAlpha (inAccentAlpha));
        inGraphics.fillRoundedRectangle (accent, 1.f);
    }
}
