#pragma once

#include <Registry.h>
#include "Tool.h"

class TileObjectTool : public Tool {
public:
    TileObjectTool(ToolManager& toolManager);
    ~TileObjectTool();

    void set() override;
    void unset() override;

    std::string getName() override;
    ToolType getType() override;

    void setObject(ObjectData* object);
    ObjectData* getObject();
private:
public:
    void update() override;

    std::string m_panelId;
    ObjectData* m_currentObject;
};
