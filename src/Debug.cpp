#include "Debug.h"
#include "constants/world.h"

Debug::Debug() = default;
Debug::~Debug() = default;

void Debug::addDebugInfo(std::string info) {}

void Debug::drawLine(Vector2 start, Vector2 end, Color colour, bool worldScale) {
    auto scale = float(worldScale ? WORLD_SCALE : 1);
    DrawLineV(start * scale, end * scale, colour);
}

void Debug::drawRect(Vector2 start, Vector2 size, Color colour, bool worldScale) {
    auto scale = float(worldScale ? WORLD_SCALE : 1);
    DrawRectangleV(start * scale, size * scale, colour);
}
