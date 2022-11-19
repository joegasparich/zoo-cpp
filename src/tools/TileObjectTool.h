#pragma once

#include <Registry.h>
#include "Tool.h"

class TileObjectTool : public Tool {
public:
    TileObjectTool(ToolManager& toolManager);
    ~TileObjectTool();

    void update() override;
    void set() override;

    std::string getName() override;
    ToolType getType() override;

    void setObject(ObjectData* object);
    ObjectData* getObject();

    void onGUI() override;

    void onInput(InputEvent* event) override;

private:
    ObjectData* currentObject;
    std::vector<ObjectData*> allObjects;
};
