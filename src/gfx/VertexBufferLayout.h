#pragma once

#include <GL/glew.h>
#include "pch.h"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalised;

    static unsigned int getSizeOfType(unsigned int type) {
        switch(type) {
            case GL_FLOAT:          return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
        }
        assert(false);
        return 0;
    }
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_elements;
    unsigned int m_stride;

public:
    VertexBufferLayout() : m_stride{0} {};

    void pushFloat(unsigned int count) {
        m_elements.push_back({ GL_FLOAT, count, GL_FALSE} );
        m_stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
    }

    void pushUInt(unsigned int count) {
        m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE} );
        m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
    }

    void pushUChar(unsigned int count) {
        m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE} );
        m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
    }

    inline const std::vector<VertexBufferElement> getElements() const { return m_elements; };
    inline unsigned int getStride() const { return m_stride; }
};
