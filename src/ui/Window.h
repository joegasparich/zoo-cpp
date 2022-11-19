#pragma once

#include "common.h"
#include "InputManager.h"

class Window {
public:
    explicit Window(const Rectangle& rect);
    Window(std::string id, const Rectangle& rect, std::function<void(Rectangle)> onUi, bool doBackground);

    virtual void doWindowContents(Rectangle r);
    virtual void handleInput(InputEvent* event);
    virtual void close();

    [[nodiscard]] const Rectangle& getRect() const;
    float getWidth() const;
    float getHeight() const;

    std::string id = "";
    bool immediate = false;
private:
    Rectangle rect;
    std::function<void(Rectangle)> onUI;
    bool doBackground;
};
