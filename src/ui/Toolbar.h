#pragma once

#include "UIComponent.h"
#include "../tools/ToolManager.h"

class Toolbar : public UIComponent {
public:
    Toolbar(ToolManager *toolManager);
    void render() override;

    void onOpen() override;

    void onClose() override;

    ~Toolbar() override;

private:
    ToolManager* m_toolManager;
};
