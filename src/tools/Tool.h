#pragma once

#include "pch.h"

class ToolManager;

enum class ToolType {
    None,
    Biome,
    Elevation,
    Wall,
    TileObject
};

class Tool {
public:
    Tool(ToolManager &toolManager);
    virtual ~Tool();

    virtual void set() = 0;
    virtual void unset() = 0;
    virtual void update() {};
    virtual void postUpdate() {};
    virtual void render() {};
    virtual std::string getName();
    virtual ToolType getType();

protected:
    ToolManager& m_toolManager;
};
