#include <constants/depth.h>
#include "Debug.h"
#include "ui/UIManager.h"

Debug::Debug() = default;
Debug::~Debug() = default;

void Debug::setup() {
    auto& debug = Debug::get();

    float lineVertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    unsigned int lineIndices[] = { 0, 1 };

    debug.m_basicLayout = std::make_unique<VertexBufferLayout>();
    debug.m_basicLayout->push<float>(2);
    debug.m_basicShader = std::make_unique<Shader>("./shaders/BasicVertex.shader", "./shaders/BasicFragment.shader");

    debug.m_va = std::make_unique<VertexArray>();
    debug.m_lineVb = std::make_unique<VertexBuffer>(lineVertices, sizeof(float) * 2 * 2);
    debug.m_va->addBuffer(*debug.m_lineVb, *debug.m_basicLayout);
    debug.m_lineIb = std::make_unique<IndexBuffer>(lineIndices, 2);

    auto component = std::make_unique<DebugInfo>();
    debug.m_debugInfo = component.get();
    UIManager::createUIComponent(std::move(component));
}

void Debug::preUpdate() {
}

void Debug::addDebugInfo(std::string info) {
    Debug::get().m_debugInfo->m_text += info + "\n";
}

// TODO: Batch these
void Debug::drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, bool inWorldPos) {
    auto& debug = Debug::get();
    auto& renderer = Renderer::get();

    // TODO: atan2 is slow as, should try figure out a different way to rotate the line
    auto rotation = std::atan2(end.y - start.y, end.x - start.x);
    auto length = glm::distance(start, end);

    debug.m_va->bind();
    debug.m_lineIb->bind();
    debug.m_basicShader->bind();

    debug.m_va->addBuffer(*debug.m_lineVb, *debug.m_basicLayout);

    auto mvp = Renderer::getMVPMatrix(start, rotation, DEPTH::DEBUG, {length, 1.0f}, inWorldPos);

    debug.m_basicShader->setUniform4f("u_Color", color);
    debug.m_basicShader->setUniformMat4f("u_MVP", mvp);

    Renderer::draw(GL_LINES, 2, GL_UNSIGNED_INT, nullptr);
}
