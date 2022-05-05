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
    // TODO: Batch renders
    Renderer::blit(*m_texture, entity->pos, 1.0f, 1.0f, true);
}

Texture &RenderComponent::getTexture() const {
    return *m_texture;
}

void RenderComponent::setTexture(Texture &texture) {
    m_texture = &texture;
}
