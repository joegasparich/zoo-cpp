#include <constants/depth.h>
#include "Renderer.h"
#include "Debug.h"
#include "constants/world.h"

void glClearError() {
    while (glGetError() != GL_NO_ERROR);
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
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthFunc(GL_LESS));

    renderer.setupBlit();

    renderer.m_camera = {{0, 0}, 1.0f};
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

    m_blitIb = std::make_unique<IndexBuffer>(indices, 6);

    m_blitShader = std::make_unique<Shader>("./shaders/TextureVertex.shader", "./shaders/TextureFragment.shader");
    m_blitColourShader = std::make_unique<Shader>("./shaders/ColourTextureVertex.shader", "./shaders/ColourTextureFragment.shader");
}

void Renderer::doRender() {
    SDL_GL_SwapWindow(Renderer::get().m_window);

    Debug::addDebugInfo("Draw Calls: " + std::to_string(Renderer::get().m_drawCallCount));
    Renderer::get().m_drawCallCount = 0;
}

void Renderer::clear() {
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::blit(BlitOptions opts) {
    auto& renderer = Renderer::get();

    std::vector<float> vertices{};
    std::unique_ptr<VertexBuffer> vb;

    renderer.m_blitVa->bind();
    renderer.m_blitIb->bind();

    if (opts.texture) {
        renderer.m_blitVa->addBuffer(*renderer.m_blitVb, *renderer.m_blitLayout);
        opts.texture->bind();
    } else if (opts.subTexture) {
        vertices = {
            0.0f, 0.0f, opts.subTexture->m_texCoord.x, opts.subTexture->m_texCoord.y,
            1.0f, 0.0f, opts.subTexture->m_texCoord.x + opts.subTexture->m_texBounds.x, opts.subTexture->m_texCoord.y,
            1.0f, 1.0f, opts.subTexture->m_texCoord.x + opts.subTexture->m_texBounds.x, opts.subTexture->m_texCoord.y + opts.subTexture->m_texBounds.y,
            0.0f, 1.0f, opts.subTexture->m_texCoord.x, opts.subTexture->m_texCoord.y + opts.subTexture->m_texBounds.y
        };
        vb = std::make_unique<VertexBuffer>(
            &vertices[0],
            sizeof(float) * 4 * 4
        );

        renderer.m_blitVa->addBuffer(*vb, *renderer.m_blitLayout);
        opts.subTexture->m_texture->bind();
    } else {
        std::cout << "A texture must be provided to blit()" << std::endl;
        return;
    }

    float depth = getDepth(opts.pos.y + opts.scale.y/2);
    if (opts.depth) {
        depth = opts.depth;
    }

    // TODO: instead of h/2 have an origin point?
    auto mvp = Renderer::getMVPMatrix(opts.pos, 0.0f, depth, glm::vec3(opts.scale, 1.0f), opts.isWorldPos);

    Shader* shader;
    if (opts.colour) {
        shader = renderer.m_blitColourShader.get();
        renderer.m_blitColourShader->bind();
        shader->setUniform3f("u_Colour", *opts.colour);
    } else {
        shader = renderer.m_blitShader.get();
        renderer.m_blitShader->bind();
    }
    shader->setUniformMat4f("u_MVP", mvp);
    shader->setUniform1i("u_Texture", 0);

    Renderer::draw(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::draw(GLenum mode, unsigned int count, GLenum type, const void *indices) {
    GL_CALL(glDrawElements(mode, count, type, indices));
    Renderer::get().m_drawCallCount++;
}

glm::mat4 Renderer::getMVPMatrix(glm::vec2 pos, float rotation, float depth, glm::vec2 scale, bool isWorldPos) {
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
    model = glm::translate(model, glm::vec3(pos, depth));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    return proj * view * model;
}

float Renderer::getDepth(float yPos) {
    return glm::clamp(yPos / LARGER_THAN_WORLD, 0.0f, 1.0f) / DEPTH::Y_SORTING_MAX;
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
    auto& camera = Renderer::get().m_camera;
    auto screenPos = worldToScreenPos(worldPos);

    return (screenPos.x > -(margin * (camera.scale * WORLD_SCALE)) && screenPos.x < WINDOW_WIDTH + (margin * (camera.scale * WORLD_SCALE))
            && screenPos.y > -(margin * (camera.scale * WORLD_SCALE)) && screenPos.y < WINDOW_HEIGHT + (margin * (camera.scale * WORLD_SCALE)));
}

void Renderer::resetCamera() {
    auto& camera = Renderer::get().m_camera;

    camera.scale = 1.0f;
    camera.pos = {0.0f, 0.0f};
}
