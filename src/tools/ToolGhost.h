#pragma once

#include "pch.h"

#include "renderer/Renderer.h"
#include "renderer/Texture.h"

enum class GhostType {
    None,
    Circle,
    Square,
    Sprite,
    SpriteSheet
};

class ToolGhost {
public:
    ToolGhost();

    void render();
    void reset();

    GhostType m_type;
    bool m_snap;
    bool m_follow;
    bool m_elevate;
    bool m_visible;
    bool m_canPlace;
    std::unique_ptr<SubTexture> m_subTexture;
    Texture* m_texture;
    glm::vec2 m_pos;
    glm::vec2 m_scale;
    glm::vec2 m_offset;
    glm::vec2 m_pivot = glm::vec2{0.5f, 0.5f};

private:
    void renderCircle();
    void renderSquare();
    void renderTexture();
    void renderSubTexture();

    std::unique_ptr<Shader> m_basicShader;
    std::unique_ptr<VertexBufferLayout> m_basicLayout;
    // Circle
    std::unique_ptr<VertexArray> m_circleVa;
};
