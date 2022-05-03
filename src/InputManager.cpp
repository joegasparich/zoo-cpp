#include <iostream>
#include "InputManager.h"
#include "util/util.h"
#include "Mediator.h"

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

bool InputManager::isMouseButtonDown(char button) const {
    return mouseButtonsDown[button];
}

bool InputManager::isMouseButtonHeld(char button) const {
    return mouseButtonsHeld[button];
}

bool InputManager::isMouseButtonUp(char button) const {
    return mouseButtonsUp[button];
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

glm::vec2& InputManager::getMousePos() {
    return mousePos;
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

    Mediator::fire(EventType::KeyPressed, SDL_GetKeyName(event.keysym.sym));

    if (registeredInputs.contains(key)) {
        auto& input{ registeredInputs.at(key) };

        inputsHeld.insert(&input);
        inputsDown.insert(&input);

//        Mediator::fire(EventType::InputPressed);
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

void InputManager::dispatchMouseDown(SDL_MouseButtonEvent &event) {
    if (event.button >= MAX_MOUSE_BUTTONS) return;

    auto button = event.button;

    mouseButtonsHeld[button] = true;
    mouseButtonsDown[button] = true;

    // Fire mediator

    // Update inputs
}

void InputManager::dispatchMouseUp(SDL_MouseButtonEvent &event) {
    if (event.button >= MAX_MOUSE_BUTTONS) return;

    auto button = event.button;

    mouseButtonsHeld[button] = false;
    mouseButtonsUp[button] = true;

    // Update inputs
}

void InputManager::dispatchMouseMove(SDL_MouseMotionEvent &event) {
    mousePos.x = (float)event.x;
    mousePos.y = (float)event.y;
}

void InputManager::clearKeys() {
    std::fill( std::begin( keysDown ), std::end( keysDown ), false );
    std::fill( std::begin( keysUp ), std::end( keysUp ), false );

    std::fill( std::begin( mouseButtonsDown ), std::end( mouseButtonsDown ), false );
    std::fill( std::begin( mouseButtonsUp ), std::end( mouseButtonsUp ), false );

    inputsDown.clear();
    inputsUp.clear();
}
