#include "Renderer.h"
#include "constants/world.h"
#include "constants/depth.h"

Renderer::Renderer() : camera() {};
Renderer::~Renderer() = default;

void Renderer::init() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    camera = {};
    camera.target = { 0 };
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void Renderer::beginDrawing() {
    BeginDrawing();
    ClearBackground(RAYWHITE); //TODO: Define our own colours

    renderedBlits = false;
}

void Renderer::endDrawing() {
    EndDrawing();
}

float Renderer::getDepth(float yPos) {
    return clamp(yPos / LARGER_THAN_WORLD, 0.0f, 1.0f) * 0.1f + DEPTH::Y_SORTING;
}

void Renderer::blit(BlitOptions opts) {
    if (renderedBlits) {
        std::cout << "Tried to blit too late" << std::endl;
        return;
    }

    blits.push_back(opts);
}

void Renderer::doBlit(BlitOptions opts) {
    // TODO: could there be precision loss here? Maybe should try keep it px values instead of fractions
    Rectangle source = {
        float(opts.texture->width) * opts.source.x,
        float(opts.texture->height) * opts.source.y,
        float(opts.texture->width) * opts.source.width,
        float(opts.texture->height) * opts.source.height
    };

    DrawTexturePro(
        *opts.texture,
        source,
        {opts.pos.x, opts.pos.y, opts.scale.x, opts.scale.y},
        multVect(opts.pivot, opts.scale),
        0.0f,
        opts.colour
    );
}

void Renderer::sortAndDrawBlits() {
    // Sort based on depth
    std::sort(blits.begin(), blits.end(), [](BlitOptions a, BlitOptions b) {
        return a.depth < b.depth;
    });

    for(BlitOptions opts : blits) {
        doBlit(opts);
    }

    blits.clear();

    renderedBlits = true;
}

Vector2 Renderer::screenToWorldPos(Vector2 screenPos) {
    return GetScreenToWorld2D(screenPos, camera) / WORLD_SCALE;
}

Vector2 Renderer::worldToScreenPos(Vector2 worldPos) {
    return GetWorldToScreen2D(worldPos * WORLD_SCALE, camera);
}

bool Renderer::isPositionOnScreen(Vector2 worldPos, float margin) {
    auto topLeft = screenToWorldPos({0, 0});
    auto bottomRight = screenToWorldPos({SCREEN_WIDTH, SCREEN_HEIGHT});

    return worldPos.x > topLeft.x && worldPos.x < bottomRight.x && worldPos.y > topLeft.y && worldPos.y < bottomRight.y;
}

bool Renderer::isRectangleOnScreen(Rectangle rect) {
    auto topLeft = screenToWorldPos({0, 0});
    auto bottomRight = screenToWorldPos({SCREEN_WIDTH, SCREEN_HEIGHT});

    return rect.x + rect.width > topLeft.x &&
           rect.x < bottomRight.x &&
           rect.y + rect.height > topLeft.y &&
           rect.y < bottomRight.y;
}

void Renderer::resetCamera() {

}
