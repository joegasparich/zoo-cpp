#include <iostream>
#include "InputManager.h"
#include "util/util.h"

InputManager::InputManager() :
        keysHeld{},
        keysDown{},
        keysUp{},
        inputsHeld{},
        inputsDown{},
        inputsUp{}
{}

bool InputManager::isKeyDown(int key) const {
    return keysDown[key];
}

bool InputManager::isKeyHeld(int key) const {
    return keysHeld[key];
}

bool InputManager::isKeyUp(int key) const {
    return keysUp[key];
}

bool InputManager::isInputDown(std::string inputName) const {
    return contains_if(inputsDown, [inputName](Input* input) { return input->name == inputName; });
}

bool InputManager::isInputHeld(std::string inputName) const {
    return contains_if(inputsHeld, [inputName](Input* input) { return input->name == inputName; });
}

bool InputManager::isInputUp(std::string inputName) const {
    return contains_if(inputsUp, [inputName](Input* input) { return input->name == inputName; });
}

void InputManager::registerInput(Input input) {
    for (auto key : input.keys) {
        if (registeredInputs.contains(key)) {
            std::cout << "Key: " << key << " already registered to " << input.name << std::endl;
            return;
        }
        registeredInputs.insert({key, input});
    }
}

void InputManager::dispatchKeyDown(SDL_KeyboardEvent& event) {
    if (event.repeat != 0) return;
    if (event.keysym.scancode >= MAX_KEYBOARD_KEYS) return;

    auto key = event.keysym.scancode;

    keysHeld[key] = true;
    keysDown[key] = true;

    if (registeredInputs.contains(key)) {
        auto& input{ registeredInputs.at(key) };

        inputsHeld.insert(&input);
        inputsDown.insert(&input);
    }
}

void InputManager::dispatchKeyUp(SDL_KeyboardEvent& event) {
    if (event.repeat != 0) return;
    if (event.keysym.scancode >= MAX_KEYBOARD_KEYS) return;

    auto key = event.keysym.scancode;

    keysHeld[key] = false;
    keysUp[key] = true;

    if (registeredInputs.contains(key)) {
        Input& input{ registeredInputs.at(key) };

        inputsHeld.erase(&input);
        inputsUp.insert(&input);
    }
}

void InputManager::clearKeys() {
    std::fill( std::begin( keysDown ), std::end( keysDown ), false );
    std::fill( std::begin( keysUp ), std::end( keysUp ), false );

    inputsDown.clear();
    inputsUp.clear();
}
