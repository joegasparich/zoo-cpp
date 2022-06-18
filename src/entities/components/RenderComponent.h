#pragma once

#include <SDL2/SDL.h>
#include <gfx/Sprite.h>
#include "pch.h"

#include "Component.h"
#include "gfx/Texture.h"
#include "util/util.h"

class RenderComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    RenderComponent(Entity* entity);
    RenderComponent(Entity* entity, std::unique_ptr<Sprite> sprite);

    void start() override;
    void render(double step) override;

    void setSprite(std::unique_ptr<Sprite> sprite);

    json save() override;
    void load(json data) override;

    glm::vec2 m_pivot = glm::vec2{0.5f, 0.5f};
private:
    std::unique_ptr<Sprite> m_sprite;
};
