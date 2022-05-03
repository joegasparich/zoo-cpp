#pragma once

#include "glm/glm.hpp"

#include "renderer/Renderer.h"

class Debug {
public:
    static Debug &get() {
        static Debug instance;
        return instance;
    }

    static void setup();
    static void drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, bool inWorldPos);
private:
    Debug();
    ~Debug();

    std::unique_ptr<VertexArray> m_lineVa;
    std::unique_ptr<VertexBuffer> m_lineVb;
    std::unique_ptr<VertexBufferLayout> m_lineLayout;
    std::unique_ptr<IndexBuffer> m_lineIb;
    std::unique_ptr<Shader> m_lineShader;
};
