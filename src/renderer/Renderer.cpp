#include "Renderer.h"
#include "../constants/world.h"

void glClearError() {
    while (glGetError() != GL_NO_ERROR);
}

void logVec(glm::vec2 vec) {
    std::cout << vec.x << ", " << vec.y << std::endl;
}

bool glLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << std::hex << error << "): " << function << " " << file << " line: " << line << std::endl;
        return false;
    }
return true;
}

Renderer::Renderer() : m_window{nullptr} {}

void Renderer::init() {
    auto& renderer = Renderer::get();

    // Set OpenGL Settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create window
    renderer.m_window = SDL_CreateWindow("Test Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    assert(renderer.m_window);

    // Create OpenGL Context
    renderer.m_glContext = SDL_GL_CreateContext(renderer.m_window);

    // Initialise GLEW
    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);

    std::cout << "Open GL Version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glEnable(GL_PRIMITIVE_RESTART));
    GL_CALL(glPrimitiveRestartIndex(PRIMITIVE_RESTART));

    renderer.setupBlit();

    renderer.m_camera = {{0, 0}, 1};
}

Renderer::~Renderer() {
    SDL_DestroyWindow(m_window);
    SDL_GL_DeleteContext(m_glContext);
}

void Renderer::setupBlit() {
    float positions[] = {
            0.0f, 0.0f,     0.0f, 0.0f, // 0
            1.0f, 0.0f,     1.0f, 0.0f, // 1
            1.0f, 1.0f,     1.0f, 1.0f, // 2
            0.0f, 1.0f,     0.0f, 1.0f  // 3
    };

    unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    m_blitVa = std::make_unique<VertexArray>();
    m_blitVb = std::make_unique<VertexBuffer>(positions, sizeof(float) * 4 * 4);

    m_blitLayout = std::make_unique<VertexBufferLayout>();
    m_blitLayout->push<float>(2);
    m_blitLayout->push<float>(2);
    m_blitVa->addBuffer(*m_blitVb, *m_blitLayout);

    m_blitIb = std::make_unique<IndexBuffer>(indices, 6);

    m_blitShader = std::make_unique<Shader>("./shaders/TextureVertex.shader", "./shaders/TextureFragment.shader");
}

void Renderer::doRender() {
    SDL_GL_SwapWindow(Renderer::get().m_window);
}

void Renderer::clear() {
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::blit(Texture &texture, glm::vec2 pos, float w, float h, bool isWorldPos) {
    auto& renderer = Renderer::get();

    renderer.m_blitVa->bind();
    renderer.m_blitIb->bind();
    renderer.m_blitShader->bind();
    texture.bind();

    auto mvp = Renderer::getMVPMatrix(pos, 0.0f, glm::vec3(w, h, 1.0f), isWorldPos);

    renderer.m_blitShader->setUniformMat4f("u_MVP", mvp);
    renderer.m_blitShader->setUniform1i("u_Texture", 0);

    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}

glm::mat4 Renderer::getMVPMatrix(glm::vec2 pos, float rotation, glm::vec2 scale, bool isWorldPos) {
    auto& renderer = Renderer::get();

    // Project to screen window
    auto proj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1.0f, 1.0f);
    // View
    auto view = glm::mat4(1.0f);
    // Move window to top left
    view = glm::translate(view, glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.0f));
    // Translate into world pos
    if (isWorldPos) {
        // Scale to camera zoom
        view = glm::scale(view,  glm::vec3(renderer.m_camera.scale * WORLD_SCALE, renderer.m_camera.scale * WORLD_SCALE, 1.0f));
        // Offset camera position
        auto cameraScreenPos = renderer.m_camera.pos;
        view = glm::translate(view,  glm::vec3(
                -cameraScreenPos.x,
                cameraScreenPos.y,
                0.0f)
        );
    }
    // Flip Y axis
    view = glm::scale(view,  glm::vec3(1.0f, -1.0f, 1.0f));
    // Model
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    return proj * view * model;
}

glm::vec2 Renderer::screenToWorldPos(glm::vec2 screenPos) {
    auto& camera = Renderer::get().m_camera;

    return ((screenPos - glm::vec2{WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}) / (camera.scale * WORLD_SCALE)) + camera.pos;
}

glm::vec2 Renderer::worldToScreenPos(glm::vec2 worldPos) {
    auto& camera = Renderer::get().m_camera;

    return (worldPos - camera.pos) * (camera.scale * WORLD_SCALE) + glm::vec2{WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
}

bool Renderer::isPositionOnScreen(glm::vec2 worldPos, float margin) {
    auto screenPos = worldToScreenPos(worldPos);

    return (screenPos.x > -(margin * WORLD_SCALE) && screenPos.x < WINDOW_WIDTH + (margin * WORLD_SCALE)
         && screenPos.y > -(margin * WORLD_SCALE) && screenPos.y < WINDOW_HEIGHT + (margin * WORLD_SCALE));
}
