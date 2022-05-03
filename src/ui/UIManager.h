#pragma once

#include <vector>
#include <map>
#include <memory>

#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_sdl.h"
#include "../lib/imgui/imgui_impl_opengl3.h"

#include "UIComponent.h"

class UIManager {
public:
    UIManager(const UIManager &) = delete;

    static UIManager &get() {
        static UIManager instance;
        return instance;
    }

    static void init();
    static bool processInput(SDL_Event* event);
    static void render();
    static std::string createUIComponent(std::unique_ptr<UIComponent> component);
    static void closeUIComponent(const std::string& componentId);

private:
    UIManager();
    ~UIManager();

    std::map<std::string, std::unique_ptr<UIComponent>> m_components;
    std::vector<std::unique_ptr<UIComponent>> m_componentsToPush;
    std::vector<std::string> m_componentsToRemove;
};
