#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "pch.h"

#include <cassert>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Sprite.h"
#include "ArrayTexture.h"
#include "FrameBuffer.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define PIXEL_SCALE 2

#define PRIMITIVE_RESTART 12345

#define MAX_BLIT_WIDTH 128
#define MAX_BLIT_HEIGHT 128
#define MAX_BLIT_TEXTURES 100

#define GL_CALL(x) glClearError();\
    x;                           \
    assert(glLogCall(#x, __FILE__, __LINE__))

void glClearError();

bool glLogCall(const char* function, const char* file, int line);

struct Camera {
    glm::vec2 pos;
    float scale;
};

struct BlitOptions {
    Texture* texture;
    Sprite* sprite;
    glm::vec2 pos;
    float depth;
    glm::vec2 scale = {1.0f, 1.0f};
    glm::vec2 pivot = {0.0f, 0.0f};
    glm::vec3 colour = {1.0f, 1.0f, 1.0f};
    unsigned int pickId = 0xFFFFFF;
};

struct ArrayTextureVertex {
    glm::vec3 pos;
    glm::vec3 texCoord;
    glm::vec3 colour;
};

class Renderer {
public:
    Renderer(const Renderer &) = delete;

    static Renderer &get() {
        static Renderer instance;
        return instance;
    }

    static void init();
    static void renderBlits();
    static void doRender();
    static void clear();
    static void blit(BlitOptions opts);
    static void draw(GLenum mode, unsigned int count, GLenum type, const void *indices);
    static int pick(glm::ivec2 screenPos);
    static glm::mat4 getMVPMatrix(glm::vec2 pos, float rotation, float depth, glm::vec2 scale, bool isWorldPos);
    static float getDepth(float yPos);

    static glm::vec2 screenToWorldPos(glm::vec2 screenPos);
    static glm::vec2 worldToScreenPos(glm::vec2 worldPos);
    static bool isPositionOnScreen(glm::vec2 worldPos, float margin = 0);

    static void resetCamera();

    Camera m_camera;
    SDL_Window *m_window;
    SDL_GLContext m_glContext;

    std::unique_ptr<FrameBuffer> m_pickFrameBuffer;

private:
    Renderer();
    ~Renderer();

    void setupBlit();

    std::unique_ptr<VertexArray> m_blitVa;
    std::unique_ptr<VertexBuffer> m_blitVb;
    std::unique_ptr<VertexBufferLayout> m_blitLayout;
    std::unique_ptr<Shader> m_blitShader;

    std::unique_ptr<ArrayTexture> m_arrayTexture;
    std::vector<ArrayTextureVertex> m_blitVertices;
    std::vector<unsigned int> m_pickIds;
    std::unique_ptr<Shader> m_pickShader;

    unsigned int m_drawCallCount;
};
