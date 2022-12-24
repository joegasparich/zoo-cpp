#include "Debug.h"
#include "constants/world.h"
#include "util/draw.h"
#include "constants/depth.h"

Debug::Debug() = default;
Debug::~Debug() = default;

void Debug::addDebugInfo(std::string info) {}

void Debug::drawLine(Vector2 start, Vector2 end, Color colour, bool worldScale) {
    auto scale = float(worldScale ? WORLD_SCALE : 1);
    DrawLineV3D(start * scale, end * scale, colour, DEPTH::DEBUG);
}

void Debug::drawRect(Vector2 start, Vector2 size, Color colour, bool worldScale) {
    auto scale = float(worldScale ? WORLD_SCALE : 1);
    DrawRectangleV3D(start * scale, size * scale, colour, DEPTH::DEBUG);
}
