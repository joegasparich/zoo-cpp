#include "ToolGhost.h"
#include "Game.h"
#include "ZooScene.h"
#include "constants/world.h"
#include "constants/depth.h"
#include "util/draw.h"

#define CIRCLE_RESOLUTION 16

ToolGhost::ToolGhost() {
    cleanup();
}

void ToolGhost::cleanup() {
    type = GhostType::None;
    snap = false;
    follow = true;
    elevate = false;
    visible = true;
    sprite = nullptr;
    spriteSheet = nullptr;
    spriteSheetIndex = 0;
    canPlace = true;
    ghostColour = GHOST_COLOUR;
    blockedColour = BLOCKED_COLOUR;

    pos = {0.0f, 0.0f};
    scale = {1.0f, 1.0f};
    radius = 1.0f;
    offset = {0.0f, 0.0f};
    pivot = {0.0f, 0.0f};
}

void ToolGhost::render() {
    if (follow) {
        pos = Root::input().getMouseWorldPos();
    }
    if (snap) {
        pos = flr(pos);
    }

    if (!visible) return;

    switch(type) {
        case GhostType::Circle: renderCircle(); break;
        case GhostType::Square: renderSquare(); break;
        case GhostType::Sprite: renderSprite(); break;
        case GhostType::SpriteSheet: renderSpriteSheet(); break;
        case GhostType::None: break;
    }
}

void ToolGhost::renderCircle() {
    std::array<Vector2, CIRCLE_RESOLUTION + 2> vertices{};

    vertices[0] = pos * WORLD_SCALE;
    for (unsigned int n = 1; n < CIRCLE_RESOLUTION + 1; ++n) {
        vertices[n] = pos;

        vertices[n].x += radius * float(sin(2.0 * M_PI * n / CIRCLE_RESOLUTION));
        vertices[n].y += radius * float(cos(2.0 * M_PI * n / CIRCLE_RESOLUTION));

        if (elevate)
            vertices[n].y -= Root::zoo()->world->elevationGrid->getElevationAtPos(vertices[n]);

        vertices[n] = vertices[n] * WORLD_SCALE;
    }
    vertices[CIRCLE_RESOLUTION + 1] = vertices[1];

    DrawLineStrip3D(&vertices[1], vertices.size() - 1, ghostColour, DEPTH::UI);
    DrawTriangleFan3D(&vertices[0], vertices.size(), ColorAlpha(ghostColour, 0.5f), DEPTH::UI);
}

void ToolGhost::renderSquare() {
    std::array<Vector2, 6> vertices{};

    vertices[0] = (pos + Vector2{scale.x/2, scale.y/2}) * WORLD_SCALE; // Centre for fan
    vertices[1] = (pos + Vector2{0,         0})         * WORLD_SCALE;
    vertices[2] = (pos + Vector2{0,         scale.y})   * WORLD_SCALE;
    vertices[3] = (pos + Vector2{scale.x,   scale.y})   * WORLD_SCALE;
    vertices[4] = (pos + Vector2{scale.x,   0})         * WORLD_SCALE;
    vertices[5] = vertices[1]; // close loop

    DrawLineStrip3D(&vertices[1], vertices.size() - 1, ghostColour, DEPTH::UI);
    DrawTriangleFan3D(&vertices[0], vertices.size(), ColorAlpha(ghostColour, 0.5f), DEPTH::UI);
}

void ToolGhost::renderSprite() {
    if (!sprite) return;

    BlitOptions opts;
    opts.texture = sprite;
    opts.pos = (pos + offset) * WORLD_SCALE;
    opts.scale = Vector2{float(opts.texture->width), float(opts.texture->height)} * PIXEL_SCALE;
    opts.depth = DEPTH::UI;
    opts.pivot = pivot;
    opts.colour = canPlace ? ghostColour : blockedColour;

    if (elevate)
        opts.pos.y += -Root::zoo()->world->elevationGrid->getElevationAtPos(pos + Vector2{0.5f, 0.5f}) * WORLD_SCALE;

    Root::renderer().blit(opts);
}

void ToolGhost::renderSpriteSheet() {
    if (!spriteSheet) return;

    BlitOptions opts;
    opts.texture = spriteSheet->texture;
    opts.source = spriteSheet->getCellBounds(spriteSheetIndex);
    opts.pos = (pos + offset) * WORLD_SCALE;
    opts.scale = Vector2{float(opts.texture->width) * opts.source.width, float(opts.texture->height) * opts.source.height} * PIXEL_SCALE;
    opts.depth = DEPTH::UI;
    opts.pivot = pivot;
    opts.colour = canPlace ? ghostColour : blockedColour;

    Root::renderer().blit(opts);
}
