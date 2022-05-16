#include <constants/depth.h>
#include "ToolGhost.h"
#include "Game.h"
#include "Zoo.h"

#define GHOST_COLOUR glm::vec3{0.4f, 0.8f, 1.0f}
#define BLOCKED_COLOUR glm::vec3{1.0f, 0.4f, 0.1f}
#define CIRCLE_RESOLUTION 16

ToolGhost::ToolGhost() {
    m_basicShader = std::make_unique<Shader>("./shaders/BasicVertex.shader", "./shaders/BasicFragment.shader");
    m_basicLayout = std::make_unique<VertexBufferLayout>();
    m_basicLayout->push<float>(2);

    m_circleVa = std::make_unique<VertexArray>();

    reset();
}

void ToolGhost::reset() {
    m_type = GhostType::None;
    m_snap = false;
    m_follow = true;
    m_elevate = false;
    m_visible = true;
    m_texture = nullptr;
    m_canPlace = true;

    m_pos = {0.0f, 0.0f};
    m_scale = {1.0f, 1.0f};
    m_offset = {0.0f, 0.0f};
    m_pivot = {0.0f, 0.0f};
}

void ToolGhost::render() {
    if (!m_visible) return;

    if (m_follow) {
        auto& input = Game::get().m_input;
        m_pos = Renderer::screenToWorldPos(input->getMousePos());
    }
    if (m_snap) {
        m_pos = glm::floor(m_pos);
    }

    switch(m_type) {
        case GhostType::Circle: renderCircle(); break;
        case GhostType::Square: renderSquare(); break;
        case GhostType::Sprite: renderTexture(); break;
        case GhostType::SpriteSheet: renderSubTexture(); break;
        case GhostType::None: break;
    }
}

void ToolGhost::renderCircle() {
    auto& renderer = Renderer::get();

    std::vector<glm::vec2> vertices{};
    vertices.resize(CIRCLE_RESOLUTION);

    for (unsigned int n = 0; n < CIRCLE_RESOLUTION; ++n) {
        vertices[n].x = (float)cos(2.0 * M_PI * n / CIRCLE_RESOLUTION);
        vertices[n].y = (float)sin(2.0 * M_PI * n / CIRCLE_RESOLUTION);

        vertices[n].y -= Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos(m_pos + vertices[n]);
    }
    vertices.insert(vertices.begin(), glm::vec2{0.0f, 0.0f});

    auto vb = VertexBuffer(&vertices[0], sizeof(float) * vertices.size() * 2);
    m_circleVa->addBuffer(vb, *m_basicLayout);

    auto mvp = Renderer::getMVPMatrix(m_pos, 0.0f, DEPTH::OVERLAY, {1.0f, 1.0f}, true);

    m_circleVa->bind();
    m_basicShader->bind();
    m_basicShader->setUniformMat4f("u_MVP", mvp);

    // Draw Outline
    {
        std::vector<unsigned int> indexList{};
        indexList.resize(CIRCLE_RESOLUTION);
        std::iota(indexList.begin(), indexList.end(), 1);
        auto ib = IndexBuffer(&indexList[0], indexList.size());
        ib.bind();

        auto colour = glm::vec4{GHOST_COLOUR, 1.0f};
        m_basicShader->setUniform4f("u_Color", colour);

        Renderer::draw(GL_LINE_LOOP, indexList.size(), GL_UNSIGNED_INT, nullptr);
    }
    // Draw Fill
    {
        std::vector<unsigned int> indexList{};
        indexList.resize(CIRCLE_RESOLUTION + 1);
        std::iota(indexList.begin(), indexList.end(), 0);
        indexList.push_back(1);
        auto ib = IndexBuffer(&indexList[0], indexList.size());
        ib.bind();

        auto colour = glm::vec4{GHOST_COLOUR, 0.5f};
        m_basicShader->setUniform4f("u_Color", colour);

        Renderer::draw(GL_TRIANGLE_FAN, indexList.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void ToolGhost::renderSquare() {

}

void ToolGhost::renderTexture() {
    if (!m_texture) return;

    auto colour = m_canPlace ? GHOST_COLOUR : BLOCKED_COLOUR;
    Renderer::blit({m_texture, nullptr, m_pos + m_offset, DEPTH::UI, m_scale, &m_pivot, &colour});
}

void ToolGhost::renderSubTexture() {
    if (!m_subTexture) return;

    auto colour = m_canPlace ? GHOST_COLOUR : BLOCKED_COLOUR;
    Renderer::blit({nullptr, m_subTexture.get(), m_pos + m_offset, DEPTH::UI, m_scale, &m_pivot, &colour});
}
