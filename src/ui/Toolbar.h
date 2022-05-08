#pragma once

#include "pch.h"

#include "UIComponent.h"
#include "tools/ToolManager.h"

class Toolbar : public UIComponent {
public:
    Toolbar(ToolManager *toolManager);
    ~Toolbar() override;
    void onClose() override;
    void render() override;

    void onOpen() override;
private:
    ToolManager* m_toolManager;
};
