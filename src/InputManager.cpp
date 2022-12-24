#include "InputManager.h"
#include "Messenger.h"
#include "Game.h"
#include "Root.h"

InputManager::InputManager() = default;
InputManager::~InputManager() = default;

void InputManager::processInput() {
    // Key events
    for (int key = 0; key < KEY_MAX; key++) {
        if (!IsKeyPressed(key) && !IsKeyReleased(key) && !IsKeyDown(key))
            continue;

        InputEvent event = { InputEventType::Key };
        event.keyDown = IsKeyPressed(key) ? key : KEY_NULL;
        event.keyUp = IsKeyReleased(key) ? key : KEY_NULL;
        event.keyHeld = IsKeyDown(key) ? key : KEY_NULL;
        event.mousePos = GetMousePosition();
        event.mouseWorldPos = Root::renderer().screenToWorldPos(event.mousePos);

        fireInputEvent(event);
    }

    // Mouse events
    for (int mouseButton = 0; mouseButton < MOUSE_BUTTON_MAX; mouseButton++) {
        if (!IsMouseButtonPressed(mouseButton) && !IsMouseButtonReleased(mouseButton) && !IsMouseButtonDown(mouseButton))
            continue;

        InputEvent event = { InputEventType::MouseButton };
        event.mouseButtonDown = IsMouseButtonPressed(mouseButton) ? mouseButton : MOUSE_BUTTON_NULL;
        event.mouseButtonUp = IsMouseButtonReleased(mouseButton) ? mouseButton : MOUSE_BUTTON_NULL;
        event.mouseButtonHeld = IsMouseButtonDown(mouseButton) ? mouseButton : MOUSE_BUTTON_NULL;
        event.mousePos = GetMousePosition();
        event.mouseWorldPos = Root::renderer().screenToWorldPos(event.mousePos);

        fireInputEvent(event);
    }

    if (GetMouseWheelMove() != 0) {
        InputEvent event = { InputEventType::MouseScroll };
        event.mouseWheelDelta = GetMouseWheelMove();
        event.mousePos = GetMousePosition();
        event.mouseWorldPos = Root::renderer().screenToWorldPos(event.mousePos);

        fireInputEvent(event);
    }
}

void InputManager::fireInputEvent(InputEvent event) {
    currentEvent = event;

    Game::get().onInput(&currentEvent);
    // TODO: actually create an input event
    Messenger::fire(EventType::InputEvent);
}

Vector2 InputManager::getMousePos () {
    return GetMousePosition();
}

Vector2 InputManager::getMouseWorldPos () {
    return Root::renderer().screenToWorldPos(getMousePos());
}

InputEvent *InputManager::getCurrentEvent() {
    return &currentEvent;
}

