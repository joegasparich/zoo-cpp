#pragma once

#include <SDL2/SDL.h>
#include "pch.h"

#include "Component.h"
#include "renderer/Texture.h"
#include "util/util.h"

class RenderComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    RenderComponent();
    explicit RenderComponent(Texture *texture);

    void start(Entity &entity) override;
    void render(double step) override;

    [[nodiscard]] Texture &getTexture() const;
    void setTexture(Texture &texture);

    glm::vec2 m_pivot;
private:
    Texture* m_texture;
};
