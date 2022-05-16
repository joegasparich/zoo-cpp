#pragma once

#include "pch.h"

#include "entities/Entity.h"
#include "world/BiomeGrid.h"
#include "tools/ToolManager.h"
#include "world/ElevationGrid.h"
#include "world/World.h"

class Stage {
public:
    Stage();

    void setup();
    void update();
    void preUpdate();
    void postUpdate();
    void reset();
    void render(double step) const;

    unsigned int registerEntity(std::unique_ptr<Entity> entity);
    void unregisterEntity(unsigned int entityId);
    Entity* getEntityById(unsigned int entityId);

    virtual ~Stage();

    std::unique_ptr<World> m_world;
    std::unique_ptr<ToolManager> m_tools;
private:
    std::unordered_map<unsigned int, std::unique_ptr<Entity>> m_entities;
    std::vector<std::unique_ptr<Entity>> m_entitiesToAdd;
    std::vector<unsigned int> m_entitiesToDelete;

    glm::vec2 m_dragStart;
    glm::vec2 m_dragCameraOrigin;

    // TODO: This needs to be saved and loaded
    unsigned int m_nextEntityId = 0;
};
