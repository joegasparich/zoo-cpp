#pragma once

#include "common.h"

#define MOUSE_BUTTON_NULL -1
#define KEY_MAX KEY_KB_MENU
#define MOUSE_BUTTON_MAX MOUSE_BUTTON_BACK

enum class InputEventType {
    Key,
    MouseButton,
    MouseScroll
};

struct InputEvent {
    InputEventType type;
    int keyDown = KEY_NULL;
    int keyUp = KEY_NULL;
    int keyHeld = KEY_NULL;
    int mouseButtonDown = MOUSE_BUTTON_NULL;
    int mouseButtonUp = MOUSE_BUTTON_NULL;
    int mouseButtonHeld = MOUSE_BUTTON_NULL;
    Vector2 mousePos;
    float mouseWheelDelta = 0;
    bool consumed = false;

    inline void consume() {
        consumed = true;
    }
};

class InputManager {
public:
    InputManager(const InputManager &) = delete;

    static InputManager &get() {
        static InputManager instance;
        return instance;
    }

    static void processInput();
    static void fireInputEvent(InputEvent event);

    static InputEvent* getCurrentEvent();

private:
    InputManager();
    ~InputManager();

    InputEvent currentEvent;
};
