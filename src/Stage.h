#pragma once

#include <vector>

#include "Entity.h"
#include "BiomeGrid.h"
#include "tools/ToolManager.h"
#include "ElevationGrid.h"
#include "world/World.h"

class Stage {
public:
    Stage();

    void setup();
    void update();
    void preUpdate();
    void postUpdate();
    void render(double step) const;

    std::unique_ptr<World> m_world;
    std::unique_ptr<ToolManager> m_tools;
private:
    std::vector<std::unique_ptr<Entity>> m_entities;
};
