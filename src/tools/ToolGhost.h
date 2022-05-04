#pragma once

#include <memory>
#include <functional>
#include "glm/glm.hpp"

#include "../renderer/Renderer.h"
#include "../renderer/Texture.h"

enum class GhostType {
    Circle,
    Square,
    Sprite
};

class ToolGhost {
public:
    ToolGhost();

    void render();
    void reset();

    GhostType type = GhostType::Circle;
    bool m_snap = false;
    bool m_follow = true;
    bool m_elevate = false;
    std::unique_ptr<Texture> m_texture;
    std::function<bool(glm::vec2 pos)> canPlace;
private:
    void renderCircle();
    void renderSquare();
    void renderTexture();

    std::unique_ptr<Shader> m_basicShader;
    std::unique_ptr<VertexBufferLayout> m_basicLayout;
    // Circle
    std::unique_ptr<VertexArray> m_circleVa;
};
