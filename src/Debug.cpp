#include <constants/depth.h>
#include "Debug.h"
#include "ui/UIManager.h"

Debug::Debug() = default;
Debug::~Debug() = default;

void Debug::setup() {
    auto& debug = Debug::get();

    debug.m_layout = std::make_unique<VertexBufferLayout>();
    debug.m_layout->push<float>(2); // Pos
    debug.m_layout->push<float>(4); // Colour
    debug.m_shader = std::make_unique<Shader>("./shaders/ColourVertex.shader", "./shaders/ColourFragment.shader");

    debug.m_va = std::make_unique<VertexArray>();

    auto component = std::make_unique<DebugInfo>();
    debug.m_debugInfo = component.get();
    UIManager::createUIComponent(std::move(component));
}

void Debug::preUpdate() {
}

void Debug::render() {
    auto& debug = Debug::get();
    auto& renderer = Renderer::get();

    // TODO: Don't update VB every frame
    auto vb = VertexBuffer{
        &debug.m_vertices[0],
        debug.m_vertices.size() * sizeof(lineVertex)
    };

    std::vector<unsigned int> indices{};
    indices.resize(debug.m_vertices.size() * 2);
    for (int i = 0; i < debug.m_vertices.size(); i++) {
        indices.push_back(i * 2);
        indices.push_back((i * 2) + 1);
    }
    auto ib = IndexBuffer{
        &indices[0],
        (unsigned int)indices.size()
    };

    debug.m_va->addBuffer(vb, *debug.m_layout);
    debug.m_va->bind();
    ib.bind();
    debug.m_shader->bind();

    auto mvp = Renderer::getMVPMatrix({0.0f, 0.0f}, 0.0f, DEPTH::DEBUG, {1.0f, 1.0f}, true);
    debug.m_shader->setUniformMat4f("u_MVP", mvp);

    Renderer::draw(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);

    debug.m_vertices.clear();
}

void Debug::addDebugInfo(std::string info) {
    Debug::get().m_debugInfo->m_text += info + "\n";
}

void Debug::drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    auto& debug = Debug::get();

    debug.m_vertices.push_back({start, colour});
    debug.m_vertices.push_back({end, colour});
}
