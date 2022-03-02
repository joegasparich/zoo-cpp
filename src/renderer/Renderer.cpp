#include "Renderer.h"

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

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    renderer.setupBlit();

    renderer.m_camera = {{0, 0}, 1};
}

Renderer::~Renderer() {
    SDL_DestroyWindow(m_window);
    SDL_GL_DeleteContext(m_glContext);
}

void Renderer::setupBlit() {
    float positions[] = {
            0.0f, 0.0f, 0.0f, 0.0f, // 0
            1.0f, 0.0f, 1.0f, 0.0f, // 1
            1.0f, 1.0f, 1.0f, 1.0f, // 2
            0.0f, 1.0f, 0.0f, 1.0f  // 3
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

    m_blitShader = std::make_unique<Shader>("./shaders/BasicVertex.shader", "./shaders/BasicFragment.shader");
}

void Renderer::doRender() {
    SDL_GL_SwapWindow(Renderer::get().m_window);
}

void Renderer::clear() {
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) {
    va.bind();
    ib.bind();
    shader.bind();

    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}

// TODO: Flag for world vs screen pos
void Renderer::blit(Texture &texture, glm::vec2 pos, float w, float h) {
    auto& renderer = Renderer::get();

    renderer.m_blitVa->bind();
    renderer.m_blitIb->bind();
    renderer.m_blitShader->bind();
    texture.bind();

    glm::mat4 proj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view,  glm::vec3(
        WINDOW_WIDTH/2,
        WINDOW_HEIGHT/2,
        0.0f)
    );
    view = glm::scale(view,  glm::vec3(renderer.m_camera.scale, renderer.m_camera.scale, 1.0f));
    view = glm::translate(view,  glm::vec3(
        -renderer.m_camera.pos.x,
        renderer.m_camera.pos.y,
        0.0f)
    );

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, -pos.y, 0));
    model = glm::scale(model, glm::vec3(w, h, 0));
    glm::mat4 mvp = proj * view * model;

    renderer.m_blitShader->setUniformMat4f("u_MVP", mvp);
    renderer.m_blitShader->setUniform1i("u_Texture", 0);

    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
