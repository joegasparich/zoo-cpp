#pragma once

#include "common.h"
#include "entities/Entity.h"
#include "world/World.h"
#include "tools/ToolManager.h"
#include "ISerialisable.h"

struct DebugSettings {
    bool cellGrid = false;
    bool elevationGrid = false;
    bool areaGrid = false;
    bool pathfindingGrid = false;
};

class Zoo : public ISerialisable {
public:
    Zoo();
    virtual ~Zoo();

    void setup();
    void update();
    void preUpdate();
    void postUpdate();
    void cleanup();
    void render(double step) const;
    void renderLate(double step) const;
    void onGUI();
    void onInput(InputEvent* event);

    unsigned int registerEntity(std::unique_ptr<Entity> entity);
    unsigned int registerEntity(std::unique_ptr<Entity> entity, unsigned int id);
    void unregisterEntity(unsigned int entityId);
    Entity* getEntityById(unsigned int entityId);

    void serialise() override;
    void SerialiseEntities(const std::string& label, std::unordered_map<unsigned int, std::unique_ptr<Entity>>& collection);

    std::unique_ptr<World> world;
    std::unique_ptr<ToolManager> tools;
    DebugSettings debugSettings;
private:
    std::unordered_map<unsigned int, std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> entitiesToAdd;
    std::vector<unsigned int> entitiesToDelete;

    Vector2 dragStart;
    Vector2 dragCameraOrigin;

    // TODO: This needs to be saved and loaded
    unsigned int nextEntityId = 0;
};
