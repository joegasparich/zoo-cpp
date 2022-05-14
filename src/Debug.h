#pragma once

#include <ui/DebugInfo.h>
#include "glm/glm.hpp"
#include "pch.h"

#include "renderer/Renderer.h"

struct lineVertex {
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
private:
    Debug();
    ~Debug();

    std::vector<lineVertex> m_vertices;
    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBufferLayout> m_layout;
    std::unique_ptr<Shader> m_shader;

    DebugInfo* m_debugInfo;
};
