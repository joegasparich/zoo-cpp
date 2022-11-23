#pragma once

#include "common.h"
#include "Component.h"
#include "util/util.h"

class RenderComponent : public Component {
public:
    typedef RenderComponent base;

    RenderComponent(Entity* entity);
    RenderComponent(Entity* entity, std::string spritePath);
    RenderComponent(Entity* entity, std::string spritePath, Rectangle source);

    COMPONENT getId() override;

    void start() override;
    void render(double step) override;

    void setSprite(std::string spritePath);
    void setSource(Rectangle source);

    void serialise() override;

    Vector2 pivot = Vector2{0.5f, 0.5f};
    Vector2 offset = Vector2{0, 0};
private:
    Texture* sprite = nullptr;
    std::string spritePath;
    Rectangle source;
};
