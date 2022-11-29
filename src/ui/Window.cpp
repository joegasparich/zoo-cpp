#include "Window.h"

#include <utility>
#include "UIManager.h"
#include "GUI.h"
#include "constants/colours.h"
#include "util/uuid.h"
#include "Root.h"

Window::Window(const Rectangle& rect) : rect(rect), doBackground(true) {
    id = uuid::generate();
}
Window::Window(std::string id, const Rectangle& rect, std::function<void(Rectangle)> onUi, bool doBackground) :
    id(std::move(id)), rect(rect), onUI(std::move(onUi)), doBackground(doBackground) {}

void Window::doWindowContents(Rectangle r) {
    if (doBackground) GUI::drawRect({0, 0, getWidth(), getHeight()}, UI_BACKGROUND_COLOUR);
    if (onUI) onUI(r);
}

void Window::handleInput(InputEvent* event) {
    if (event->consumed) return;

    doWindowContents(rect);

    // Consume event if it's a mouse button down on the window
    if (pointInRect(rect, event->mousePos) && event->mouseButtonDown != MOUSE_BUTTON_NULL) {
        event->consume();
    }
}

void Window::close() {
    Root::ui().closeWindow(id);
}

const Rectangle& Window::getRect() const {
    return rect;
}

float Window::getWidth() const {
    return rect.width;
}

float Window::getHeight() const {
    return rect.height;
}
