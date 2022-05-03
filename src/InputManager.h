#pragma once

#include <SDL2/SDL.h>
#include "glm/vec2.hpp"
#include <string>
#include <set>
#include <map>

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
    [[nodiscard]] glm::vec2& getMousePos();
    void registerInput(Input input);

    void dispatchKeyDown(SDL_KeyboardEvent& event);
    void dispatchKeyUp(SDL_KeyboardEvent& event);
    void dispatchMouseDown(SDL_MouseButtonEvent& event);
    void dispatchMouseUp(SDL_MouseButtonEvent& event);
    void dispatchMouseMove(SDL_MouseMotionEvent& event);
    void clearKeys();

private:
    std::map<int, Input> registeredInputs;

    bool keysHeld[MAX_KEYBOARD_KEYS];
    bool keysDown[MAX_KEYBOARD_KEYS];
    bool keysUp[MAX_KEYBOARD_KEYS];

    bool mouseButtonsHeld[MAX_MOUSE_BUTTONS];
    bool mouseButtonsDown[MAX_MOUSE_BUTTONS];
    bool mouseButtonsUp[MAX_MOUSE_BUTTONS];

    glm::vec2 mousePos;

    std::set<Input*> inputsHeld;
    std::set<Input*> inputsDown;
    std::set<Input*> inputsUp;
};
