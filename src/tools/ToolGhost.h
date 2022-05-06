#pragma once

#include "pch.h"

#include "renderer/Renderer.h"
#include "renderer/Texture.h"

enum class GhostType {
    None,
    Circle,
    Square,
    Sprite
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
    std::unique_ptr<Texture> m_texture;
    std::function<bool(glm::vec2 pos)> m_canPlace;
private:
    void renderCircle();
    void renderSquare();
    void renderTexture();

    std::unique_ptr<Shader> m_basicShader;
    std::unique_ptr<VertexBufferLayout> m_basicLayout;
    // Circle
    std::unique_ptr<VertexArray> m_circleVa;
};
