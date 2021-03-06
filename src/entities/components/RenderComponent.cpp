#include "RenderComponent.h"
#include "gfx/Renderer.h"
#include "Game.h"
#include "constants/world.h"

COMPONENT RenderComponent::getId() { return RENDER_COMPONENT; }
COMPONENT RenderComponent::getType() { return RENDER_COMPONENT; }
std::set<COMPONENT> RenderComponent::getRequiredComponents() {
    return {};
}

RenderComponent::RenderComponent(Entity *entity) : Component(entity) {}
RenderComponent::RenderComponent(Entity *entity, std::unique_ptr<Sprite> sprite) : Component(entity) {
    if (sprite) {
        setSprite(std::move(sprite));
    }
}

void RenderComponent::start() {
    Component::start();
}

void RenderComponent::render(double step) {
    BlitOptions opts;
    opts.sprite = m_sprite.get();
    opts.pos = m_entity->m_pos;
    opts.depth = Renderer::getDepth(m_entity->m_pos.y);
    opts.pivot = m_pivot;
    opts.pickId = m_entity->getId();

    Renderer::blit(opts);
}


void RenderComponent::setSprite(std::unique_ptr<Sprite> sprite) {
    m_sprite = std::move(sprite);
}

json RenderComponent::save() {
    auto saveData = Component::save();
    saveData.push_back({"sprite", json({
       {"path", m_sprite->m_texture->m_image->m_filePath},
       {"texCoord", m_sprite->m_texCoord},
       {"texBounds", m_sprite->m_texBounds}
    })});
    saveData.push_back({"pivot", m_pivot});

    return saveData;
}

void RenderComponent::load(json data) {
    Component::load(data);
    m_sprite = std::make_unique<Sprite>(
        AssetManager::loadTexture(AssetManager::loadImage(data.at("sprite").at("path").get<std::string>())),
        data.at("sprite").at("texCoord").get<glm::vec2>(),
        data.at("sprite").at("texBounds").get<glm::vec2>()
    );
    data.at("pivot").get_to(m_pivot);
}
