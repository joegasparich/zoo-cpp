#include "pch.h"

#include "InputManager.h"
#include "util/util.h"
#include "Messenger.h"

InputManager::InputManager() :
        m_keysHeld{},
        m_keysDown{},
        m_keysUp{},
        m_inputsHeld{},
        m_inputsDown{},
        m_inputsUp{},
        m_mouseButtonsHeld{},
        m_mouseButtonsDown{},
        m_mouseButtonsUp{},
        m_scrollAmount{}
{}

bool InputManager::isKeyDown(int key) const {
    return m_keysDown[key];
}

bool InputManager::isKeyHeld(int key) const {
    return m_keysHeld[key];
}

bool InputManager::isKeyUp(int key) const {
    return m_keysUp[key];
}

bool InputManager::isMouseButtonDown(char button) const {
    return m_mouseButtonsDown[button];
}

bool InputManager::isMouseButtonHeld(char button) const {
    return m_mouseButtonsHeld[button];
}

bool InputManager::isMouseButtonUp(char button) const {
    return m_mouseButtonsUp[button];
}

bool InputManager::isInputDown(std::string inputName) const {
    return contains_if(m_inputsDown, [inputName](Input* input) { return input->name == inputName; });
}

bool InputManager::isInputHeld(std::string inputName) const {
    return contains_if(m_inputsHeld, [inputName](Input* input) { return input->name == inputName; });
}

bool InputManager::isInputUp(std::string inputName) const {
    return contains_if(m_inputsUp, [inputName](Input* input) { return input->name == inputName; });
}

const glm::vec2& InputManager::getMousePos() {
    return m_mousePos;
}

const int InputManager::getMouseScroll() {
    return m_scrollAmount;
}

void InputManager::registerInput(Input input) {
    for (auto key : input.keys) {
        if (m_registeredInputs.contains(key)) {
            std::cout << "Key: " << key << " already registered to " << input.name << std::endl;
            return;
        }
        m_registeredInputs.insert_or_assign(key, input);
    }
}

void InputManager::dispatchKeyDown(SDL_KeyboardEvent& event) {
    if (event.repeat != 0) return;
    if (event.keysym.scancode >= MAX_KEYBOARD_KEYS) return;

    auto key = event.keysym.scancode;

    m_keysHeld[key] = true;
    m_keysDown[key] = true;

    Messenger::fire(EventType::KeyPressed, SDL_GetKeyName(event.keysym.sym));

    if (m_registeredInputs.contains(key)) {
        auto& input{m_registeredInputs.at(key) };

        m_inputsHeld.insert(&input);
        m_inputsDown.insert(&input);

//        Messenger::fire(EventType::InputPressed);
    }
}

void InputManager::dispatchKeyUp(SDL_KeyboardEvent& event) {
    if (event.repeat != 0) return;
    if (event.keysym.scancode >= MAX_KEYBOARD_KEYS) return;

    auto key = event.keysym.scancode;

    m_keysHeld[key] = false;
    m_keysUp[key] = true;

    if (m_registeredInputs.contains(key)) {
        Input& input{m_registeredInputs.at(key) };

        m_inputsHeld.erase(&input);
        m_inputsUp.insert(&input);
    }
}

void InputManager::dispatchMouseDown(SDL_MouseButtonEvent &event) {
    if (event.button >= MAX_MOUSE_BUTTONS) return;

    auto button = event.button;

    m_mouseButtonsHeld[button] = true;
    m_mouseButtonsDown[button] = true;

    // Fire mediator

    // Update inputs
}

void InputManager::dispatchMouseUp(SDL_MouseButtonEvent &event) {
    if (event.button >= MAX_MOUSE_BUTTONS) return;

    auto button = event.button;

    m_mouseButtonsHeld[button] = false;
    m_mouseButtonsUp[button] = true;

    // Update inputs
}

void InputManager::dispatchMouseMove(SDL_MouseMotionEvent &event) {
    m_mousePos.x = (float)event.x;
    m_mousePos.y = (float)event.y;
}

void InputManager::dispatchMouseScroll(SDL_MouseWheelEvent &event) {
    m_scrollAmount = event.y;
}

void InputManager::clearKeys() {
    std::fill(std::begin(m_keysDown ), std::end(m_keysDown ), false );
    std::fill(std::begin(m_keysUp ), std::end(m_keysUp ), false );

    std::fill(std::begin(m_mouseButtonsDown ), std::end(m_mouseButtonsDown ), false );
    std::fill(std::begin(m_mouseButtonsUp ), std::end(m_mouseButtonsUp ), false );

    m_inputsDown.clear();
    m_inputsUp.clear();

    m_scrollAmount = 0;
}
