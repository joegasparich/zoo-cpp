#include "createComponentById.h"

#include "../Entity.h"
#include "RenderComponent.h"
#include "TileObjectComponent.h"
#include "ElevationComponent.h"
#include "PhysicsComponent.h"
#include "MoveComponent.h"
#include "PathFollowComponent.h"
#include "AreaPathFollowComponent.h"

Component* createComponentById(COMPONENT id, Entity *entity) {
    switch(id) {
        case RENDER_COMPONENT:
            entity->addComponent(std::make_unique<RenderComponent>(entity));
            return entity->getComponent<RenderComponent>();
        case TILE_OBJECT_COMPONENT:
            entity->addComponent(std::make_unique<TileObjectComponent>(entity));
            return entity->getComponent<TileObjectComponent>();
        case ELEVATION_COMPONENT:
            entity->addComponent(std::make_unique<ElevationComponent>(entity));
            return entity->getComponent<ElevationComponent>();
        case PHYSICS_COMPONENT:
            entity->addComponent(std::make_unique<PhysicsComponent>(entity));
            return entity->getComponent<PhysicsComponent>();
        case MOVE_COMPONENT:
            entity->addComponent(std::make_unique<MoveComponent>(entity));
            return entity->getComponent<MoveComponent>();
        case PATH_FOLLOW_COMPONENT:
            entity->addComponent(std::make_unique<PathFollowComponent>(entity));
            return entity->getComponent<PathFollowComponent>();
        case AREA_PATH_FOLLOW_COMPONENT:
            entity->addComponent(std::make_unique<AreaPathFollowComponent>(entity));
            return entity->getComponent<AreaPathFollowComponent>();
        default:
            return nullptr;
    }
}
