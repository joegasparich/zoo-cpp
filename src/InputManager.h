#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <set>
#include <map>

#define MAX_KEYBOARD_KEYS 350

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
    [[nodiscard]] bool isInputDown(std::string inputName) const;
    [[nodiscard]] bool isInputHeld(std::string inputName) const;
    [[nodiscard]] bool isInputUp(std::string inputName) const;
    void registerInput(Input input);

    void dispatchKeyDown(SDL_KeyboardEvent& event);
    void dispatchKeyUp(SDL_KeyboardEvent& event);
    void clearKeys();

private:
    std::map<int, Input> registeredInputs;

    bool keysHeld[MAX_KEYBOARD_KEYS];
    bool keysDown[MAX_KEYBOARD_KEYS];
    bool keysUp[MAX_KEYBOARD_KEYS];

    std::set<Input*> inputsHeld;
    std::set<Input*> inputsDown;
    std::set<Input*> inputsUp;
};
