#pragma once

#include "common.h"
#include "ui/Window.h"
#include "InputManager.h"

enum class UIEvent {
    Draw,
    Input
};

class UIManager {
public:
    UIManager();
    ~UIManager();

    void init();
    void onInput(InputEvent* event);
    void preRender();
    void render();
    void postRender();

    std::string pushWindow(std::unique_ptr<Window> window);
    void closeWindow(const std::string& id);
    void bringWindowToFront(const std::string& id);
    Window* getWindow(const std::string& id);
    bool isWindowOpen(const std::string& id);
    void doImmediateWindow(const std::string& id, const Rectangle& initialRect, std::function<void(Rectangle rect)> onUI, bool doBackground = true);
    void setCursor(int c);
    Rectangle getAbsRect(Rectangle rect) const;
    bool isMouseOverRect(Rectangle rect);

    UIEvent currentEvent;
    Rectangle currentDrawBounds;

private:
    // TODO: list might be better here for reordering
    std::list<std::unique_ptr<Window>> windowStack;
    std::map<std::string, Window*> openWindowMap;
    std::vector<std::unique_ptr<Window>> windowsToOpen;
    std::set<std::string> windowsToClose;
    std::set<std::string> immediateWindows;

    int cursor;
    std::string hoveredWindowId;
    std::string currentWindowId;
};
