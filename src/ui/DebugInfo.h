#pragma once

#include <imgui/imgui.h>
#include "UIComponent.h"

class DebugInfo : public UIComponent {
public:
    DebugInfo();
    ~DebugInfo() override;

    void render() override;
    void onOpen() override;
    void onClose() override;

    std::string m_text;
};
