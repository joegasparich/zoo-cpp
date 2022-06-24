#include <constants/depth.h>
#include <util/jmath.h>
#include <util/colour.h>
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
    renderer.m_pickFrameBuffer = std::make_unique<FrameBuffer>();
    renderer.m_pickShader = std::make_unique<Shader>("./shaders/PickVertex.shader", "./shaders/PickFragment.shader");

    renderer.m_camera = {{0, 0}, 1.0f};
}

Renderer::~Renderer() {
    SDL_DestroyWindow(m_window);
    SDL_GL_DeleteContext(m_glContext);
}

void Renderer::setupBlit() {
    m_blitVa = std::make_unique<VertexArray>();
    // TODO: Do we need more than 1000 blits? probably
    m_blitVb = std::make_unique<VertexBuffer>(nullptr, sizeof(ArrayTextureVertex) * 4 * 1000, true);

    m_blitLayout = std::make_unique<VertexBufferLayout>();
    m_blitLayout->pushFloat(3); // pos
    m_blitLayout->pushFloat(3); // texCoord
    m_blitLayout->pushFloat(3); // colour

    m_blitVa->addBuffer(*m_blitVb, *m_blitLayout);

    m_blitShader = std::make_unique<Shader>("./shaders/ArrayTextureVertex.shader", "./shaders/ArrayTextureFragment.shader");

    m_arrayTexture = std::make_unique<ArrayTexture>(MAX_BLIT_WIDTH, MAX_BLIT_HEIGHT, MAX_BLIT_TEXTURES);
}

void Renderer::doRender() {
    auto& renderer = Renderer::get();

    renderer.m_blitVertices.clear();

    SDL_GL_SwapWindow(Renderer::get().m_window);

    Debug::addDebugInfo("Draw Calls: " + std::to_string(renderer.m_drawCallCount));
    renderer.m_drawCallCount = 0;
}

void Renderer::clear() {
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::blit(BlitOptions opts) {
    auto& renderer = Renderer::get();

    auto texCoord = opts.sprite ? opts.sprite->m_texCoord : glm::vec2{0.0f, 0.0f};
    auto texBounds = opts.sprite ? opts.sprite->m_texBounds : glm::vec2{1.0f, 1.0f};
    auto image = opts.sprite ? opts.sprite->m_texture->m_image : opts.texture->m_image;

    auto width = (float)image->m_width * (float)texBounds.x * (float)PIXEL_SCALE / (float)WORLD_SCALE;
    auto height = (float)image->m_height * (float)texBounds.y * (float)PIXEL_SCALE / (float)WORLD_SCALE;

    texCoord = jmath::multVect(texCoord, glm::vec2{(float)image->m_width / (float)MAX_BLIT_WIDTH, (float)image->m_height / (float)MAX_BLIT_HEIGHT});
    texBounds = jmath::multVect(texBounds, glm::vec2{(float)image->m_width / (float)MAX_BLIT_WIDTH, (float)image->m_height / (float)MAX_BLIT_HEIGHT});

    auto texId = opts.sprite ?
            renderer.m_arrayTexture->pushTexture(*opts.sprite->m_texture->m_image) :
            renderer.m_arrayTexture->pushTexture(*opts.texture->m_image);

    if (texId >= MAX_BLIT_TEXTURES) {
        std::cout << "Error loading texture" << std::endl;
        return;
    }

    auto offset = -glm::vec2{opts.pivot.x * width, opts.pivot.y * height};

    renderer.m_blitVertices.push_back({{opts.pos + offset + glm::vec2{0.0f,         0.0f},          opts.depth}, {texCoord.x,               texCoord.y,               (float)texId}, opts.colour});
    renderer.m_blitVertices.push_back({{opts.pos + offset + glm::vec2{1.0f * width, 0.0f},          opts.depth}, {texCoord.x + texBounds.x, texCoord.y,               (float)texId}, opts.colour});
    renderer.m_blitVertices.push_back({{opts.pos + offset + glm::vec2{1.0f * width, 1.0f * height}, opts.depth}, {texCoord.x + texBounds.x, texCoord.y + texBounds.y, (float)texId}, opts.colour});
    renderer.m_blitVertices.push_back({{opts.pos + offset + glm::vec2{0.0f,         1.0f * height}, opts.depth}, {texCoord.x,               texCoord.y + texBounds.y, (float)texId}, opts.colour});

    renderer.m_pickIds.push_back(opts.pickId);
}

void Renderer::renderBlits() {
    auto& renderer = Renderer::get();

    std::vector<std::array<unsigned int, 6>> indices{};

    for (int i = 0; i < renderer.m_blitVertices.size() / 4; i++) {
        unsigned int base = i * 4;
        indices.push_back({base, base + 1, base + 2,
                           base + 2, base + 3, base});
    }
    auto ib = IndexBuffer{&indices[0], (unsigned int)indices.size() * 6};

    renderer.m_blitVb->updateData(&renderer.m_blitVertices[0], renderer.m_blitVertices.size() * sizeof(ArrayTextureVertex), 0);
    renderer.m_blitVa->bind();
    ib.bind();
    renderer.m_blitShader->bind();
    renderer.m_arrayTexture->bind();

    auto mvp = Renderer::getMVPMatrix({0.0f, 0.0f}, 0.0f, 0.0f, glm::vec3(1.0f), true);
    renderer.m_blitShader->setUniformMat4f("u_MVP", mvp);
    renderer.m_blitShader->setUniform1i("u_Textures", 0);

    Renderer::draw(GL_TRIANGLES, indices.size() * 6, GL_UNSIGNED_INT, nullptr);

    // Pick
    renderer.m_pickFrameBuffer->bind();
    GL_CALL(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    for (int i = 0; i < renderer.m_blitVertices.size(); i++) {
        renderer.m_blitVertices[i].colour = glm::vec3{intToColour256(renderer.m_pickIds[i / 4])} / 255.0f;
    }
    renderer.m_blitVb->updateData(&renderer.m_blitVertices[0], renderer.m_blitVertices.size() * sizeof(ArrayTextureVertex), 0);
    renderer.m_pickShader->bind();
    renderer.m_pickShader->setUniformMat4f("u_MVP", mvp);
    renderer.m_pickShader->setUniform1i("u_Textures", 0);

    Renderer::draw(GL_TRIANGLES, indices.size() * 6, GL_UNSIGNED_INT, nullptr);

    renderer.m_pickFrameBuffer->unbind();
    renderer.m_pickIds.clear();
}

void Renderer::draw(GLenum mode, unsigned int count, GLenum type, const void *indices) {
    GL_CALL(glDrawElements(mode, count, type, indices));
    Renderer::get().m_drawCallCount++;
}

int Renderer::pick(glm::ivec2 screenPos) {
    auto& renderer = Renderer::get();

    renderer.m_pickFrameBuffer->bind();
    GL_CALL(glFlush());
    GL_CALL(glFinish());
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    unsigned char data[4];
    GL_CALL(glReadPixels(screenPos.x, WINDOW_HEIGHT - screenPos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data));

    renderer.m_pickFrameBuffer->unbind();

    auto id = colour256ToInt({data[0], data[1], data[2]});
    if (id == 0xFFFFFF) return -1;

    return id;
}

glm::mat4 Renderer::getMVPMatrix(glm::vec2 pos, float rotation, float depth, glm::vec2 scale, bool isWorldPos) {
    auto& renderer = Renderer::get();

    // Project to screen window
    auto proj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1.0f, 1.0f);
    // View
    auto view = glm::mat4(1.0f);
    // Move window to top left
    view = glm::translate(view, glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.0f));
    // Translate into world m_pos
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
    return glm::clamp(yPos / LARGER_THAN_WORLD, 0.0f, 1.0f) * 0.1f + DEPTH::Y_SORTING;
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
