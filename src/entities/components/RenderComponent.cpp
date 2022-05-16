#include "RenderComponent.h"
#include "renderer/Renderer.h"
#include "Game.h"
#include "constants/world.h"

COMPONENT RenderComponent::getId() { return RENDER_COMPONENT; }
COMPONENT RenderComponent::getType() { return RENDER_COMPONENT; }
std::set<COMPONENT> RenderComponent::getRequiredComponents() {
    return {};
}

RenderComponent::RenderComponent(Entity *entity, Texture *texture) : Component(entity) {
    if (texture) {
        setTexture(*texture);
    }
}
RenderComponent::RenderComponent(Entity *entity, std::unique_ptr<SubTexture> subTexture) : Component(entity) {
    if (subTexture) {
        setSubTexture(std::move(subTexture));
    }
}

void RenderComponent::start() {
    Component::start();
}

void RenderComponent::render(double step) {
    BlitOptions opts;
    opts.texture = m_texture;
    opts.subTexture = m_subTexture.get();
    opts.pos = m_entity->m_pos;
    opts.depth = Renderer::getDepth(m_entity->m_pos.y);
    opts.pivot = m_pivot;
    opts.pickId = m_entity->getId();

    Renderer::blit(opts);
}

Texture &RenderComponent::getTexture() const {
    return *m_texture;
}

void RenderComponent::setTexture(Texture &texture) {
    m_subTexture = nullptr;
    m_texture = &texture;
}

void RenderComponent::setSubTexture(std::unique_ptr<SubTexture> subTexture) {
    m_texture = nullptr;
    m_subTexture = std::move(subTexture);
}
