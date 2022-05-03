#include "Debug.h"
#include "constants/world.h"

Debug::Debug() = default;
Debug::~Debug() = default;

void Debug::setup() {
    auto& debug = Debug::get();

    float lineVertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    unsigned int lineIndices[] = {0, 1 };

    debug.m_lineVa = std::make_unique<VertexArray>();
    debug.m_lineVb = std::make_unique<VertexBuffer>(lineVertices, sizeof(float) * 2 * 2);
    debug.m_lineLayout = std::make_unique<VertexBufferLayout>();
    debug.m_lineLayout->push<float>(2);
    debug.m_lineVa->addBuffer(*debug.m_lineVb, *debug.m_lineLayout);
    debug.m_lineIb = std::make_unique<IndexBuffer>(lineIndices, 2);
    debug.m_lineShader = std::make_unique<Shader>("./shaders/BasicVertex.shader", "./shaders/BasicFragment.shader");

}

void Debug::drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, bool inWorldPos) {
    auto& debug = Debug::get();
    auto& renderer = Renderer::get();

    auto rotation = std::atan2(end.y - start.y, end.x - start.x);
    auto length = glm::distance(start, end);

    debug.m_lineVa->bind();
    debug.m_lineIb->bind();
    debug.m_lineShader->bind();

    auto proj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1.0f, 1.0f);
    auto view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, WINDOW_HEIGHT, 0.0f));
    if (inWorldPos) {
        view = glm::scale(view,  glm::vec3(renderer.m_camera.scale, renderer.m_camera.scale, 1.0f));
        auto cameraScreenPos = renderer.m_camera.pos * (float)WORLD_SCALE;
        view = glm::translate(view,  glm::vec3(
                -cameraScreenPos.x,
                cameraScreenPos.y,
                0.0f)
        );
    }
    view = glm::scale(view,  glm::vec3(1.0f, -1.0f, 1.0f));
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(start, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(length, 1.0f, 1.0f));
    auto mvp = proj * view * model;
    debug.m_lineShader->setUniform4f("u_Color", color.r, color.g, color.b, color.a);
    debug.m_lineShader->setUniformMat4f("u_MVP", mvp);

    GL_CALL(glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr));
}
