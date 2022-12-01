#pragma once

#include "common.h"
#include "InputManager.h"

#define WINDOW_TITLE_HEIGHT 18

class Window {
public:
    explicit Window(const Rectangle& rect);
    Window(std::string id, const Rectangle& rect, std::function<void(Rectangle)> onUi, bool doBackground);

    virtual void doWindowContents();
    virtual void handleInput(InputEvent* event);
    virtual void close();

    float getWidth();
    float getHeight();

    std::string id;
    Rectangle absRect;
    bool immediate = false;
    bool draggable = false;
    std::string title;
protected:
    Rectangle getRect();

    bool headerHovered = false;
private:
    std::function<void(Rectangle)> onUI;
    bool doBackground;

    bool isDragging = false;
    Vector2 dragPos;
};
