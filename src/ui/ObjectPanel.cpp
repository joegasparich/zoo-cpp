#include "ObjectPanel.h"

ObjectPanel::ObjectPanel(ToolManager &toolManager, TileObjectTool &objectTool) :
        UIComponent("Object"),
        m_toolManager{toolManager},
        m_tileObjectTool{objectTool},
        m_allObjects{Registry::getAllObjects()}
{
    m_isResizeable = false;
    m_isScrollable = false;
}
ObjectPanel::~ObjectPanel() = default;

void ObjectPanel::render() {
    UIComponent::render();

    ImGui::SetWindowSize({200, 200});

    auto currentObject = m_tileObjectTool.getObject();

    for (auto object : m_allObjects) {
        ImGui::Text("%s", object->name.c_str());

        if (object->name == currentObject->name) ImGui::PushStyleColor(ImGuiCol_Button, {1, 1, 1, 1});
        if (object->image) {
            if (ImGui::ImageButton(
                    (void*)(intptr_t)AssetManager::loadTexture(object->image)->getRendererId(),
                    {32.0f, 32.0f},
                    {0.0f, 0.0f},
                    {1.0f, 1.0f},
                    5.0f
            )) {
                m_tileObjectTool.setObject(object);
            }
        } else {
            if (ImGui::Button(
                object->name.substr(0, 1).c_str(),
                {32.0f, 32.0f}
            )) {
                m_tileObjectTool.setObject(object);
            }
        }
        if (object->name == currentObject->name) ImGui::PopStyleColor();
    }
}

void ObjectPanel::onOpen() {}

void ObjectPanel::onClose() {
    if (!m_externallyClosed) {
        m_toolManager.setTool(ToolType::None);
    }
}
