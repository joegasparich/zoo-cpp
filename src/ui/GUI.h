
#pragma once

#include "common.h"

enum class AlignMode {
    TopLeft,
    TopCentre,
    TopRight,
    CentreLeft,
    Centre,
    CentreRight,
    BottomLeft,
    BottomCentre,
    BottomRight
};

inline float GAP_SMALL = 10;

class GUI {
public:
    GUI(const GUI &) = delete;

    static GUI &get() {
        static GUI instance;
        return instance;
    }

    // State
    static Color TextColour;
    static AlignMode TextAlign;
    static int FontSize;

    // Draw
    static void drawRect(Rectangle rect, Color col);
    static void drawBorder(Rectangle rect, float thickness, Color col);
    static void drawTexture(Rectangle rect, Texture* texture, Color col = WHITE);
    static void drawSubTexture(Rectangle rect, Texture* texture, Rectangle source);
    static void drawTextureNPatch(Rectangle rect, Texture* texture, int cornerSize, Color col = WHITE);
    static void drawText(Rectangle rect, const std::string& text);
    static void highlightMouseover(Rectangle rect);

    // Interactable
    static bool clickableArea(Rectangle rect);
    static bool hoverArea(Rectangle rect);

    // Widget
    static bool buttonText(Rectangle rect, const std::string& text);
    static bool buttonText(Rectangle rect, const std::string& text, Color colour);
private:
    GUI();
    ~GUI();
};
