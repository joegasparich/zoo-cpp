#include "./createComponentById.h"

#include "../Entity.h"
#include "RenderComponent.h"
#include "TileObjectComponent.h"

Component* createComponentById(COMPONENT id, Entity *entity) {
    switch(id) {
        case RENDER_COMPONENT:
            entity->addComponent(std::make_unique<RenderComponent>(entity));
            return entity->getComponent<RenderComponent>();
        case TILE_OBJECT_COMPONENT:
            entity->addComponent(std::make_unique<TileObjectComponent>(entity));
            return entity->getComponent<TileObjectComponent>();
    }
}
