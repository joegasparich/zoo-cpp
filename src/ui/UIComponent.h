#pragma once

#include "pch.h"

class UIComponent {
public:
    UIComponent(std::string name);
    virtual ~UIComponent();

    virtual void render();
    void postRender();
    virtual void onOpen() = 0;
    virtual void onClose() = 0;

    std::string m_id;
    bool m_isOpen = true;
    bool m_externallyClosed = false;
protected:
    std::string m_name = "Component";
    bool m_showHeader = true;
    bool m_isDraggable = true;
    bool m_isResizeable = true;
    bool m_isScrollable = true;
};
