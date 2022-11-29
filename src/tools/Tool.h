#pragma once

#include "common.h"
#include "InputManager.h"

class ToolManager;

enum class ToolType {
    None,
    Biome,
    Elevation,
    Wall,
    Door,
    Path,
    TileObject,
    Delete
};

class Tool {
public:
    Tool(ToolManager &toolManager);
    virtual ~Tool();

    virtual void set() {};
    virtual void unset() {};
    virtual void preUpdate() {};
    virtual void update() {};
    virtual void postUpdate() {};
    virtual void render() {};
    virtual void onGUI() {};
    virtual void onInput(InputEvent* event) {};
    virtual std::string getName();
    virtual ToolType getType();

protected:
    ToolManager& toolManager;
};
