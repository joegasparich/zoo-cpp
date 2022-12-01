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
    Vector2 mouseWorldPos;
    float mouseWheelDelta = 0;
    bool consumed = false;

    inline void consume() {
        consumed = true;
    }
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    void processInput();
    void fireInputEvent(InputEvent event);

    Vector2 getMousePos();
    Vector2 getMouseWorldPos();
    InputEvent* getCurrentEvent();

private:
    InputEvent currentEvent;
};
