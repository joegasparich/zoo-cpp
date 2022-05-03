#pragma once

#include <string>

class ToolManager;

enum class ToolType {
    None,
    Biome,
    Elevation
};

class Tool {
public:
    Tool(ToolManager &toolManager);
    virtual ~Tool();

    virtual void update() = 0;
    virtual void postUpdate() = 0;
    virtual std::string getName();
    virtual ToolType getType();

protected:
    ToolManager& m_toolManager;
};
