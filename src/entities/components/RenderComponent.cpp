#include "RenderComponent.h"
#include "renderer/Renderer.h"
#include "Game.h"
#include "constants/world.h"

COMPONENT RenderComponent::getId() { return RENDER_COMPONENT; }
COMPONENT RenderComponent::getType() { return RENDER_COMPONENT; }
std::set<COMPONENT> RenderComponent::getRequiredComponents() {
    return {};
}

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
    auto height = (float)m_texture->m_image->m_height;
    auto width = (float)m_texture->m_image->m_width;
    auto max = std::max(height, width);
    auto dimensions = glm::vec2{width/max, height/max};
    auto offset = glm::vec2{m_pivot.x * dimensions.x, m_pivot.y * dimensions.y};

    // TODO: Batch renders
    Renderer::blit({ m_texture, m_entity->m_pos - offset, dimensions });
}

Texture &RenderComponent::getTexture() const {
    return *m_texture;
}

void RenderComponent::setTexture(Texture &texture) {
    m_texture = &texture;
}
