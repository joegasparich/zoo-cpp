#include "UIManager.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "util/uuid.h"
#include "Root.h"

UIManager::UIManager() : windowStack(), openWindowMap(), windowsToOpen(), windowsToClose() {
    currentEvent = UIEvent::Draw;
    currentDrawBounds = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; // TODO: Dynamic
}
UIManager::~UIManager() = default;

void UIManager::init() {
    TraceLog(LOG_INFO, "Initialising UI");
}

void UIManager::render() {
    currentEvent = UIEvent::Draw;
    Root::sceneManager().getCurrentScene()->onGUI();

    // Clear closed immediate windows
    windowStack.erase(std::remove_if(windowStack.begin(), windowStack.end(),
        [&](auto& window) {
            return window->immediate && !immediateWindows.contains(window->id);
        }), windowStack.end());
    immediateWindows.clear();

    // Render windows
    for (auto& window : windowStack) {
        currentDrawBounds = window->getRect();
        window->doWindowContents(currentDrawBounds);
    }
    currentDrawBounds = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
}

void UIManager::onInput(InputEvent* event) {
    currentEvent = UIEvent::Input;
    Root::sceneManager().getCurrentScene()->onGUI();

    // Clear closed immediate windows
    windowStack.erase(std::remove_if(windowStack.begin(), windowStack.end(),
        [&](auto& window) {
            return window->immediate && !immediateWindows.contains(window->id);
        }), windowStack.end());
    immediateWindows.clear();

    // Reverse so that top windows consume events first
    for (auto it = windowStack.rbegin(); it != windowStack.rend(); it++) {
        auto window = it->get();

        currentDrawBounds = window->getRect();
        window->handleInput(event);
    }
    currentDrawBounds = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
}

void UIManager::postUpdate() {
    windowStack.erase(std::remove_if(windowStack.begin(), windowStack.end(),
        [&](auto& window) {
            return windowsToClose.contains(window->id);
        }), windowStack.end());

    for (auto& window : windowsToOpen) {
        windowStack.push_back(std::move(window));
    }

    windowsToOpen.clear();
    windowsToClose.clear();
}

std::string UIManager::pushWindow(std::unique_ptr<Window> window) {
    if (window->id.empty())
        window->id = uuid::generate();

    auto id = window->id;
    windowsToOpen.push_back(std::move(window));
    openWindowMap.insert({id, windowsToOpen.back().get()});

    return id;
}

void UIManager::closeWindow(const std::string& id) {
    openWindowMap.erase(id);
    windowsToClose.insert(id);
}

Window* UIManager::getWindow(const std::string& id) {
    return openWindowMap.at(id);
}

bool UIManager::isWindowOpen(const std::string& id) {
    return openWindowMap.contains(id);
}

void UIManager::doImmediateWindow(const std::string& id, const Rectangle& initialRect, std::function<void(Rectangle)> onUI, bool doBackground) {
    bool found = false;
    for (auto& window : windowStack) {
        if (window->id == id) {
            found = true;

            // Do we need to update onUI function here?
        }
    }

    if (!found) {
        std::unique_ptr<Window> window;

        window = std::make_unique<Window>(id, initialRect, onUI, doBackground);
        window->immediate = true;

        pushWindow(std::move(window));
    }

    immediateWindows.insert(id);
}
