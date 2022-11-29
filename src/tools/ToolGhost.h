#pragma once

#include "common.h"

#include "Renderer.h"
#include "SpriteSheet.h"

#define GHOST_COLOUR Color{102, 204, 255, 255}
#define BLOCKED_COLOUR Color{255, 102, 26, 255}

enum class GhostType {
    None,
    Circle,
    Square,
    Sprite,
    SpriteSheet
};

class ToolGhost {
public:
    ToolGhost();

    void render();
    void cleanup();

    GhostType type;
    bool snap;
    bool follow;
    bool elevate;
    bool visible;
    bool canPlace;
    Texture* sprite;
    SpriteSheet* spriteSheet;
    unsigned int spriteSheetIndex;
    Vector2 pos;
    Vector2 scale;
    float radius;
    Vector2 offset;
    Vector2 pivot = {0.5f, 0.5f};
    Color ghostColour = GHOST_COLOUR;
    Color blockedColour = BLOCKED_COLOUR;

private:
    void renderCircle();
    void renderSquare();
    void renderSprite();
    void renderSpriteSheet();
};
