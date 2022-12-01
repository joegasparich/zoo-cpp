#include "Window.h"

#include <utility>
#include "UIManager.h"
#include "GUI.h"
#include "constants/colours.h"
#include "util/uuid.h"
#include "Root.h"

#define WINDOW_NPATCH "img/ui/window.png"
#define WINDOW_HEADER_NPATCH "img/ui/window_header.png"

Window::Window(const Rectangle& rect) : absRect(rect), doBackground(true) {
    id = uuid::generate();
}
Window::Window(std::string id, const Rectangle& rect, std::function<void(Rectangle)> onUi, bool doBackground) :
    id(std::move(id)), absRect(rect), onUI(std::move(onUi)), doBackground(doBackground) {}

void Window::doWindowContents() {
    if (isDragging) {
        auto newPos = Root::input().getMousePos() - dragPos;
        absRect.x = newPos.x;
        absRect.y = newPos.y;
    }

    if (doBackground) {
        auto backgroundTexture = !title.empty() || draggable ? WINDOW_HEADER_NPATCH : WINDOW_NPATCH;
        GUI::drawTextureNPatch(getRect(), Root::assetManager().getTexture(backgroundTexture), 20);
    }

    Rectangle headerRect{0, 0, getWidth(), WINDOW_TITLE_HEIGHT};
    if (!title.empty()) {
        GUI::TextAlign = AlignMode::Centre;
        GUI::drawText(headerRect, title);
        GUI::TextAlign = AlignMode::TopLeft;
    }

    headerHovered = false;
    if (GUI::hoverArea(headerRect)) {
        headerHovered = true;

        if (draggable)
            Root::ui().setCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    if (onUI) onUI(getRect());
}

void Window::handleInput(InputEvent* event) {
    doWindowContents();

    // Dragging
    if (draggable) {
        if (headerHovered && event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
            dragPos = event->mousePos - Vector2{absRect.x, absRect.y};
            isDragging = true;
            Root::ui().bringWindowToFront(id);
        }
        if (isDragging && event->mouseButtonUp == MOUSE_BUTTON_LEFT) {
            isDragging = false;
        }
    }
}

void Window::close() {
    Root::ui().closeWindow(id);
}

float Window::getWidth () {
    return absRect.width;
}

float Window::getHeight () {
    return absRect.height;
}

Rectangle Window::getRect() {
    return {0, 0, absRect.width, absRect.height};
}
