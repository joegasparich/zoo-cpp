#pragma once

#include "common.h"

#include "Renderer.h"
#include "SpriteSheet.h"

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

private:
    void renderCircle();
    void renderSquare();
    void renderSprite();
    void renderSpriteSheet();
};
