#include "RenderComponent.h"

#include <utility>
#include "Renderer.h"
#include "entities/Entity.h"
#include "constants/world.h"
#include "Root.h"

RenderComponent::RenderComponent(Entity *entity) : Component(entity) {}
RenderComponent::RenderComponent(Entity *entity, std::string _spritePath)
    : RenderComponent(entity, std::move(_spritePath), Rectangle{0, 0, 1, 1}) {}
RenderComponent::RenderComponent(Entity* entity, std::string _spritePath, Rectangle source)
    : Component(entity) {
    setSprite(std::move(_spritePath));
    setSource(source);
}

COMPONENT RenderComponent::getId() { return RENDER_COMPONENT; }

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
    opts.pickId = entity->id;
    opts.colour = overrideColour;

    Root::renderer().blit(opts);

    overrideColour = WHITE;
}

void RenderComponent::setSprite(std::string path) {
    sprite = Root::assetManager().getTexture(path);
    spritePath = path;
}

void RenderComponent::setSource(Rectangle _source) {
    source = _source;
}

void RenderComponent::serialise() {
    Component::serialise();

    Root::saveManager().SerialiseValue("spritePath", spritePath);
    Root::saveManager().SerialiseValue("source", source);
    Root::saveManager().SerialiseValue("pivot", pivot);

    if (Root::saveManager().mode == SerialiseMode::Loading)
        sprite = Root::assetManager().getTexture(spritePath);
}
