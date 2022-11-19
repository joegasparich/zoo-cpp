#include "RenderComponent.h"

#include <utility>
#include "Renderer.h"
#include "entities/Entity.h"
#include "constants/world.h"
#include "AssetManager.h"
#include "Root.h"

COMPONENT RenderComponent::getId() { return RENDER_COMPONENT; }
COMPONENT RenderComponent::getType() { return RENDER_COMPONENT; }
std::set<COMPONENT> RenderComponent::getRequiredComponents() {
    return {};
}

RenderComponent::RenderComponent(Entity *entity) : Component(entity) {}
RenderComponent::RenderComponent(Entity *entity, std::string _spritePath) : RenderComponent(
    entity, std::move(_spritePath), Rectangle{0, 0, 1, 1}
) {}

RenderComponent::RenderComponent(Entity* entity, std::string _spritePath, Rectangle source) : Component(entity) {
    setSprite(std::move(_spritePath));
    setSource(source);
}

void RenderComponent::start() {
    Component::start();
}

void RenderComponent::render(double step) {
    BlitOptions opts;
    opts.texture = sprite;
    opts.source = source;
    opts.pos = (entity->pos + offset) * WORLD_SCALE;
    opts.scale = Vector2{float(sprite->width) * source.width, float(sprite->height) * source.height} * PIXEL_SCALE;
    opts.depth = Root::renderer().getDepth(entity->pos.y);
    opts.pivot = pivot;
    opts.pickId = entity->getId();

    Root::renderer().blit(opts);
}


void RenderComponent::setSprite(std::string path) {
    sprite = Root::assetManager().getTexture(path);
    spritePath = path;
}

void RenderComponent::setSource(Rectangle _source) {
    source = _source;
}

json RenderComponent::save() {
    auto saveData = Component::save();
    saveData.push_back({"spritePath", spritePath});
    saveData.push_back({"source", source});
    saveData.push_back({"pivot", pivot});

    return saveData;
}

void RenderComponent::load(json data) {
    Component::load(data);
    data.at("spritePath").get_to(spritePath);
    data.at("source").get_to(source);
    data.at("pivot").get_to(pivot);
    sprite = Root::assetManager().getTexture(spritePath);
}
