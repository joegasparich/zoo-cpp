#include "Debug.h"
#include "constants/depth.h"
#include "ui/UIManager.h"

#define VERTEX_LIMIT 10000 // 240kb

Debug::Debug() = default;
Debug::~Debug() = default;

void Debug::setup() {
    auto& debug = Debug::get();

    debug.m_layout = std::make_unique<VertexBufferLayout>();
    debug.m_layout->pushFloat(2); // Pos
    debug.m_layout->pushFloat(4); // Colour
    debug.m_shader = std::make_unique<Shader>("./shaders/ColourVertex.shader", "./shaders/ColourFragment.shader");

    debug.m_va = std::make_unique<VertexArray>();

    debug.m_vb = std::make_unique<VertexBuffer>(nullptr, sizeof(SimpleVertex) * VERTEX_LIMIT, true);
    debug.m_va->addBuffer(*debug.m_vb.get(), *debug.m_layout);

    auto component = std::make_unique<DebugInfo>();
    debug.m_debugInfo = component.get();
    UIManager::createUIComponent(std::move(component));
}

void Debug::preUpdate() {
}

void Debug::render() {
    auto& debug = Debug::get();
    auto& renderer = Renderer::get();

    debug.m_va->bind();
    debug.m_shader->bind();
    auto mvp = Renderer::getMVPMatrix({0.0f, 0.0f}, 0.0f, DEPTH::DEBUG, {1.0f, 1.0f}, true);
    debug.m_shader->setUniformMat4f("u_MVP", mvp);

    // Lines
    debug.m_vb->updateData(&debug.m_lineVertices[0], debug.m_lineVertices.size() * sizeof(SimpleVertex) * 2, 0);

    std::vector<std::array<unsigned int, 2>> lineIndices{};
    lineIndices.reserve(debug.m_lineVertices.size());
    for (int i = 0; i < debug.m_lineVertices.size(); i++) {
        unsigned int base = i * 2;
        lineIndices.push_back({base, base + 1});
    }
    auto ib = IndexBuffer{
        &lineIndices[0],
        (unsigned int)lineIndices.size() * 2
    };
    ib.bind();

    Renderer::draw(GL_LINES, lineIndices.size() * 2, GL_UNSIGNED_INT, nullptr);

    debug.m_lineVertices.clear();

    // Rectangles
    debug.m_vb->updateData(&debug.m_rectVertices[0], debug.m_rectVertices.size() * sizeof(SimpleVertex) * 4, 0);

    std::vector<std::array<unsigned int, 6>> rectIndices{};
    rectIndices.reserve(debug.m_rectVertices.size());
    for (int i = 0; i < debug.m_rectVertices.size(); i++) {
        unsigned int base = i * 4;
        rectIndices.push_back({base, base + 1, base + 2,
                           base + 2, base + 3, base});
    }
    auto ib2 = IndexBuffer{
        &rectIndices[0],
        (unsigned int)rectIndices.size() * 6
    };
    ib2.bind();

    Renderer::draw(GL_TRIANGLES, rectIndices.size() * 6, GL_UNSIGNED_INT, nullptr);

    debug.m_rectVertices.clear();
}

void Debug::addDebugInfo(std::string info) {
    Debug::get().m_debugInfo->m_text += info + "\n";
}

void Debug::drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    auto& debug = Debug::get();

    debug.m_lineVertices.push_back({SimpleVertex{start, colour}, SimpleVertex{end, colour}});
}


void Debug::drawRect(glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    auto& debug = Debug::get();

    debug.m_rectVertices.push_back({
        SimpleVertex{{start.x, start.y}, colour},
        SimpleVertex{{end.x, start.y}, colour},
        SimpleVertex{{end.x, end.y}, colour},
        SimpleVertex{{start.x, end.y}, colour}
    });
}
