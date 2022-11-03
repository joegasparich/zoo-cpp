#pragma once

#include "ui/DebugInfo.h"
#include "glm/glm.hpp"
#include "pch.h"

#include "gfx/Renderer.h"

struct SimpleVertex {
    glm::vec2 pos;
    glm::vec4 colour;
};

class Debug {
public:
    static Debug &get() {
        static Debug instance;
        return instance;
    }

    static void setup();
    static void preUpdate();
    static void render();
    static void addDebugInfo(std::string info);

    static void drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 colour);
    static void drawRect(glm::vec2 start, glm::vec2 end, glm::vec4 colour);
private:
    Debug();
    ~Debug();

    std::vector<std::array<SimpleVertex, 2>> m_lineVertices;
    std::vector<std::array<SimpleVertex, 4>> m_rectVertices;
    std::unique_ptr<VertexBuffer> m_vb;
    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBufferLayout> m_layout;
    std::unique_ptr<Shader> m_shader;

    DebugInfo* m_debugInfo;
};
