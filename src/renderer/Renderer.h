#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cassert>
#include <iostream>

#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/matrix_transform.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define GL_CALL(x) glClearError();\
    x;                           \
    assert(glLogCall(#x, __FILE__, __LINE__))

void glClearError();

bool glLogCall(const char* function, const char* file, int line);

struct Camera {
    glm::vec2 pos;
    float scale;
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
    static void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
    static void blit(Texture &texture, glm::vec2 pos, float w, float h);

    Camera m_camera;

private:
    Renderer();
    ~Renderer();

    void setupBlit();

    SDL_Window *m_window;
    SDL_GLContext m_glContext;

    std::unique_ptr<VertexArray> m_blitVa;
    std::unique_ptr<VertexBuffer> m_blitVb;
    std::unique_ptr<VertexBufferLayout> m_blitLayout;
    std::unique_ptr<IndexBuffer> m_blitIb;
    std::unique_ptr<Shader> m_blitShader;
};
