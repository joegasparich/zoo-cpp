#include "RenderComponent.h"
#include "../renderer/Renderer.h"
#include "../Game.h"
#include "../constants/world.h"

RenderComponent::RenderComponent() {}

RenderComponent::RenderComponent(Texture *texture) {
    if (texture) {
        setTexture(*texture);
    }
}

void RenderComponent::start(Entity &entity) {
    Component::start(entity);
}

void RenderComponent::render(double step) {
    Renderer::blit(*m_texture, entity->pos * (float)WORLD_SCALE, 100, 100);
}

Texture &RenderComponent::getTexture() const {
    return *m_texture;
}

void RenderComponent::setTexture(Texture &texture) {
    m_texture = &texture;
}
