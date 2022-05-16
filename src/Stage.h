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

    std::string registerEntity(std::unique_ptr<Entity> entity);
    void unregisterEntity(std::string entityId);
    Entity* getEntityById(std::string entityId);

    virtual ~Stage();

    std::unique_ptr<World> m_world;
    std::unique_ptr<ToolManager> m_tools;
private:
    std::unordered_map<std::string, std::unique_ptr<Entity>> m_entities;
    std::vector<std::unique_ptr<Entity>> m_entitiesToAdd;
    std::vector<std::string> m_entitiesToDelete;

    glm::vec2 m_dragStart;
    glm::vec2 m_dragCameraOrigin;
};
