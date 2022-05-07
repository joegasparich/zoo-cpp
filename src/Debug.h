#pragma once

#include <ui/DebugInfo.h>
#include "glm/glm.hpp"
#include "pch.h"

#include "renderer/Renderer.h"

class Debug {
public:
    static Debug &get() {
        static Debug instance;
        return instance;
    }

    static void setup();
    static void preUpdate();
    static void addDebugInfo(std::string info);

    static void drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, bool inWorldPos);
private:
    Debug();
    ~Debug();

    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBuffer> m_lineVb;
    std::unique_ptr<IndexBuffer> m_lineIb;
    std::unique_ptr<VertexBufferLayout> m_basicLayout;
    std::unique_ptr<Shader> m_basicShader;

    DebugInfo* m_debugInfo;
};
