#pragma once

#include "common.h"
#include "Component.h"
#include "util/util.h"

class RenderComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    RenderComponent(Entity* entity);
    RenderComponent(Entity* entity, std::string spritePath);
    RenderComponent(Entity* entity, std::string spritePath, Rectangle source);

    void start() override;
    void render(double step) override;

    void setSprite(std::string spritePath);
    void setSource(Rectangle source);

    json save() override;
    void load(json data) override;

    Vector2 pivot = Vector2{0.5f, 0.5f};
    Vector2 offset = Vector2{0, 0};
private:
    Texture* sprite = nullptr;
    std::string spritePath;
    Rectangle source;
};
