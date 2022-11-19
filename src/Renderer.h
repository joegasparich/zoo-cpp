#pragma once

#include "common.h"

// TODO: Move to constants file
#define WINDOW_TITLE "ZOO"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define PIXEL_SCALE 2

struct BlitOptions {
    Texture* texture;
    Rectangle source = {0, 0, 1, 1};
    Vector2 pos;
    Vector2 scale = {1.0f, 1.0f};
    Vector2 pivot = {0.0f, 0.0f};
    float depth = 0;
    Color colour = WHITE;
    unsigned int pickId = 0xFFFFFF;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init();
    void beginDrawing();
    void endDrawing();
    void sortAndDrawBlits();
    float getDepth(float yPos);
    void blit(BlitOptions opts);

    Vector2 screenToWorldPos(Vector2 screenPos);
    Vector2 worldToScreenPos(Vector2 worldPos);
    bool isPositionOnScreen(Vector2 worldPos, float margin = 0);
    bool isRectangleOnScreen(Rectangle rect);

    void resetCamera();

    Camera2D camera;

private:
    void doBlit(BlitOptions opts);

    std::vector<BlitOptions> blits;
    bool renderedBlits = false;
};
