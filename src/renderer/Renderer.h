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
#include "SubTexture.h"
#include "ArrayTexture.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define PRIMITIVE_RESTART 12345

#define MAX_BLIT_WIDTH 64
#define MAX_BLIT_HEIGHT 64
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
    glm::vec2 pos;
    glm::vec2 scale;
    SubTexture* subTexture;
    glm::vec3* colour;
    float depth;
};

struct BlitVertex {
    glm::vec3 pos;
    glm::vec3 texCoord;
//    glm::vec3 colour;
};

class Renderer {
public:
    Renderer(const Renderer &) = delete;

    static Renderer &get() {
        static Renderer instance;
        return instance;
    }

    static void init();
    static void doRender();
    static void clear();
    static void blit(BlitOptions opts);
    static void draw(GLenum mode, unsigned int count, GLenum type, const void *indices);
    static glm::mat4 getMVPMatrix(glm::vec2 pos, float rotation, float depth, glm::vec2 scale, bool isWorldPos);
    static float getDepth(float yPos);

    static glm::vec2 screenToWorldPos(glm::vec2 screenPos);
    static glm::vec2 worldToScreenPos(glm::vec2 worldPos);
    static bool isPositionOnScreen(glm::vec2 worldPos, float margin = 0);

    static void resetCamera();

    Camera m_camera;
    SDL_Window *m_window;
    SDL_GLContext m_glContext;

private:
    Renderer();
    ~Renderer();

    void setupBlit();
    void renderBlitArray();

    std::unique_ptr<VertexArray> m_blitVa;
    std::unique_ptr<VertexBuffer> m_blitVb;
    std::unique_ptr<VertexBufferLayout> m_blitLayout;
    std::unique_ptr<Shader> m_blitShader;

    std::unique_ptr<ArrayTexture> m_arrayTexture;
    std::vector<BlitVertex> m_blitVertices;

    unsigned int m_drawCallCount;
};
