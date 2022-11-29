#include "GUI.h"
#include "constants/colours.h"
#include "UIManager.h"
#include "Root.h"

Color GUI::TextColour = BLACK;
AlignMode GUI::TextAlign = AlignMode::TopLeft;
int GUI::FontSize = 10;
Color highlightColor = {255, 255, 255, 150};

// Util
Rectangle getAbsRect(Rectangle rect) {
    return {
        Root::ui().currentDrawBounds.x + rect.x,
        Root::ui().currentDrawBounds.y + rect.y,
        rect.width,
        rect.height
    };
}
bool mouseOverUI(Rectangle rect) {
    return pointInRect(getAbsRect(rect), InputManager::getMousePos());
}
Rectangle maintainAspectRatio(Rectangle rect, Texture* texture, Rectangle source = {0, 0, 1, 1}) {
    float texWidth = texture->width * source.width;
    float texHeight = texture->height * source.height;

    if (texWidth < texHeight) {
        auto width = rect.width * (texWidth / texHeight);
        return {
            rect.x + (rect.width - width) / 2.0f,
            rect.y,
            width,
            rect.height
        };
    } else if (texWidth > texHeight) {
        auto height = rect.height * (texHeight / texWidth);
        return {
            rect.x,
            rect.y + (rect.height - height) / 2.0f,
            rect.width,
            height
        };
    }

    return rect;
}

// Draw Functions
void GUI::drawRect(Rectangle rect, Color col) {
    auto absRect = getAbsRect(rect);

    if (Root::ui().currentEvent == UIEvent::Draw)
        DrawRectangle(absRect.x, absRect.y, absRect.width, absRect.height, col);
}

void GUI::drawBorder(Rectangle rect, float thickness, Color col) {
    drawRect({rect.x, rect.y, rect.width, thickness}, col);                           // top
    drawRect({rect.x, rect.y + rect.height - thickness, rect.width, thickness}, col); // bottom
    drawRect({rect.x, rect.y, thickness, rect.height}, col);                          // left
    drawRect({rect.x + rect.width - thickness, rect.y, thickness, rect.height}, col); // right
}

void GUI::drawTexture(Rectangle rect, Texture* texture) {
    if (!texture) return;
    auto absRect = getAbsRect(rect);

    DrawTexturePro(
        *texture,
        Rectangle{0.0f, 0.0f, float(texture->width), float(texture->height)},
        maintainAspectRatio(absRect, texture),
        Vector2{0, 0},
        0.0f,
        WHITE
    );
}

void GUI::drawSubTexture(Rectangle rect, Texture* texture, Rectangle source) {
    if (!texture) return;
    auto absRect = getAbsRect(rect);
    Rectangle dimensions = {
        float(texture->width * source.x),
        float(texture->height * source.y),
        float(texture->width * source.width),
        float(texture->height * source.height)
    };

    DrawTexturePro(
        *texture,
        dimensions,
        maintainAspectRatio(absRect, texture, source),
        Vector2{0, 0},
        0.0f,
        WHITE
    );
}

void GUI::drawText(Rectangle rect, const std::string& text) {
    auto absRect = getAbsRect(rect);
    Vector2 drawPos = {absRect.x, absRect.y};
    int textWidth = MeasureText(text.c_str(), FontSize);

    switch (TextAlign) {
        case AlignMode::TopLeft:      drawPos = {absRect.x,                                   absRect.y}; break;
        case AlignMode::TopCentre:    drawPos = {absRect.x + (absRect.width - textWidth) / 2, absRect.y}; break;
        case AlignMode::TopRight:     drawPos = {absRect.x + (absRect.width - textWidth),     absRect.y}; break;
        case AlignMode::CentreLeft:   drawPos = {absRect.x,                                   absRect.y + (absRect.height - FontSize) / 2}; break;
        case AlignMode::Centre:       drawPos = {absRect.x + (absRect.width - textWidth) / 2, absRect.y + (absRect.height - FontSize) / 2}; break;
        case AlignMode::CentreRight:  drawPos = {absRect.x + (absRect.width - textWidth),     absRect.y + (absRect.height - FontSize) / 2}; break;
        case AlignMode::BottomLeft:   drawPos = {absRect.x,                                   absRect.y + (absRect.height - FontSize)}; break;
        case AlignMode::BottomCentre: drawPos = {absRect.x + (absRect.width - textWidth) / 2, absRect.y + (absRect.height - FontSize)}; break;
        case AlignMode::BottomRight:  drawPos = {absRect.x + (absRect.width - textWidth),     absRect.y + (absRect.height - FontSize)}; break;
    }

    DrawText(text.c_str(), drawPos.x, drawPos.y, GUI::FontSize, TextColour);
}

// Input Functions
bool GUI::clickableArea(Rectangle rect) {
    if (Root::ui().currentEvent == UIEvent::Input) {
        auto event = InputManager::getCurrentEvent();

        if (event->mouseButtonUp == MOUSE_BUTTON_LEFT && mouseOverUI(rect)) {
            return true;
        }
    }

    return false;
}

// Widgets
bool GUI::buttonText(Rectangle rect, const std::string& text) {
    return buttonText(rect, text, UI_BUTTON_COLOUR);
}

bool GUI::buttonText(Rectangle rect, const std::string& text, Color colour) {
    drawRect(rect, colour);
    highlightMouseover(rect);
    drawText(rect, text);

    return clickableArea(rect);
}

void GUI::highlightMouseover(Rectangle rect) {
    if (mouseOverUI(rect))
        drawRect(rect, highlightColor);
}
