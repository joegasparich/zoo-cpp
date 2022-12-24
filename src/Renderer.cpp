#include "Renderer.h"

#include <rlgl.h>
#include "util/draw.h"
#include "constants/world.h"
#include "constants/depth.h"

#define MIN_ZOOM 0.5f
#define MAX_ZOOM 10.0f
#define ZOOM_RATE 0.005f
#define CAMERA_SPEED 2

Renderer::Renderer() : camera() {};
Renderer::~Renderer() = default;

void Renderer::init() {
    TraceLog(LOG_TRACE, "Initialising Renderer");
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    discardAlphaFragShader = LoadShader(NULL, ASSETS_PATH"/shaders/discard_alpha.fs");

    camera = {};
    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.fovy = SCREEN_HEIGHT / zoom;
    camera.position = {0.0f, 0.0f, -10.0f};
    camera.up = { 0.0f, -1.0f, 0.0f };

    dragStart = GetMousePosition();
    dragCameraOrigin = {camera.target.x, camera.target.y};

    rlEnableDepthTest();
}

void Renderer::update () {

    // Camera movement
    // TODO: Refactor this out somewhere and use input manager

    // TODO: Why is there a lag spike when you first start moving the camera
    float inputHorizontal = float(IsKeyDown(KEY_RIGHT)) - float(IsKeyDown(KEY_LEFT));
    float inputVertical = float(IsKeyDown(KEY_DOWN)) - float(IsKeyDown(KEY_UP));

    camera.position.x += inputHorizontal * CAMERA_SPEED / zoom;
    camera.position.y += inputVertical * CAMERA_SPEED / zoom;
    camera.target = { camera.position.x, camera.position.y, 0 };

    // TODO
//    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
//        dragStart = GetMousePosition();
//        dragCameraOrigin = camera.target;
//    }
//    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
//        camera.target = dragCameraOrigin + (dragStart - GetMousePosition()) / (camera.zoom * WORLD_SCALE);
//    }

    // Camera zoom
    float zoomDelta = GetMouseWheelMove() / 10.0f;

    if (IsKeyDown(KEY_COMMA)) zoomDelta += ZOOM_RATE;
    if (IsKeyDown(KEY_PERIOD)) zoomDelta -= ZOOM_RATE;

    // TODO: Zoom towards mouse

    // Do zoom
    auto normalisedZoomLog = normalise(log(zoom), log(MIN_ZOOM), log(MAX_ZOOM));
    zoom = exp(lerp(log(MIN_ZOOM), log(MAX_ZOOM), normalisedZoomLog + zoomDelta));
    zoom = Clamp(zoom, MIN_ZOOM, MAX_ZOOM);

    camera.fovy = SCREEN_HEIGHT / zoom;
}

void Renderer::beginDrawing() {
    BeginDrawing();
    ClearBackground(BLUE); //TODO: Define our own colours

    BeginShaderMode(discardAlphaFragShader);
}

void Renderer::endDrawing() {
    EndShaderMode();

    EndDrawing();
}

float Renderer::getDepth(float yPos) {
    return clamp(yPos / LARGER_THAN_WORLD, 0.0f, 1.0f) * -1 + DEPTH::Y_SORTING;
}

void Renderer::blit(BlitOptions opts) {
    // TODO: could there be precision loss here? Maybe should try keep it px values instead of fractions
    Rectangle source = {
        float(opts.texture->width) * opts.source.x,
        float(opts.texture->height) * opts.source.y,
        float(opts.texture->width) * opts.source.width,
        float(opts.texture->height) * opts.source.height
    };

    auto pivot = multVect(opts.pivot, opts.scale);

    DrawTexturePro3D(
        *opts.texture,
        source,
        {opts.pos.x, opts.pos.y, opts.scale.x, opts.scale.y},
        {pivot.x, pivot.y, 0},
        0.0f,
        opts.depth,
        opts.colour
    );
}

Vector2 Renderer::screenToWorldPos(Vector2 screenPos) {
    auto cameraCenter = xy(camera.position * zoom) - Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

    return (screenPos + cameraCenter) / (WORLD_SCALE * zoom);
}

// TODO: make sure this is correct since this is literally AI generated
Vector2 Renderer::worldToScreenPos(Vector2 worldPos) {
    return worldPos * (WORLD_SCALE * zoom) - xy(camera.position * zoom) + Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
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
