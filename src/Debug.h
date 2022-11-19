#pragma once

#include "common.h"
#include "Renderer.h"

class Debug {
public:
    static Debug &get() {
        static Debug instance;
        return instance;
    }

    static void addDebugInfo(std::string info);

    static void drawLine(Vector2 start, Vector2 end, Color colour, bool worldScale);
    static void drawRect(Vector2 start, Vector2 end, Color colour, bool worldScale);
private:
    Debug();
    ~Debug();
};
