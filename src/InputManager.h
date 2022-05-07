#pragma once

#include <SDL2/SDL.h>
#include "pch.h"

#define MAX_KEYBOARD_KEYS 350
#define MAX_MOUSE_BUTTONS 5

struct Input {
    std::string name;
    std::set<int> keys;
};

class InputManager {
public:
    InputManager();

    [[nodiscard]] bool isKeyDown(int key) const;
    [[nodiscard]] bool isKeyHeld(int key) const;
    [[nodiscard]] bool isKeyUp(int key) const;
    [[nodiscard]] bool isMouseButtonDown(char button) const;
    [[nodiscard]] bool isMouseButtonHeld(char button) const;
    [[nodiscard]] bool isMouseButtonUp(char button) const;
    [[nodiscard]] bool isInputDown(std::string inputName) const;
    [[nodiscard]] bool isInputHeld(std::string inputName) const;
    [[nodiscard]] bool isInputUp(std::string inputName) const;
    [[nodiscard]] const glm::vec2& getMousePos();
    [[nodiscard]] const int getMouseScroll();
    void registerInput(Input input);

    void dispatchKeyDown(SDL_KeyboardEvent& event);
    void dispatchKeyUp(SDL_KeyboardEvent& event);
    void dispatchMouseDown(SDL_MouseButtonEvent& event);
    void dispatchMouseUp(SDL_MouseButtonEvent& event);
    void dispatchMouseMove(SDL_MouseMotionEvent& event);
    void dispatchMouseScroll(SDL_MouseWheelEvent& event);
    void clearKeys();

private:
    std::map<int, Input> m_registeredInputs;

    bool m_keysHeld[MAX_KEYBOARD_KEYS];
    bool m_keysDown[MAX_KEYBOARD_KEYS];
    bool m_keysUp[MAX_KEYBOARD_KEYS];

    bool m_mouseButtonsHeld[MAX_MOUSE_BUTTONS];
    bool m_mouseButtonsDown[MAX_MOUSE_BUTTONS];
    bool m_mouseButtonsUp[MAX_MOUSE_BUTTONS];

    glm::vec2 m_mousePos;
    int m_scrollAmount;

    std::set<Input*> m_inputsHeld;
    std::set<Input*> m_inputsDown;
    std::set<Input*> m_inputsUp;
};
