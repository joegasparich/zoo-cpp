#pragma once

#include <memory>
#include <SDL2/SDL.h>

#include "Component.h"
#include "../renderer/Texture.h"
#include "../util/util.h"

class RenderComponent : public Component {
public:
    RenderComponent();
    explicit RenderComponent(Texture *texture);

    void start(Entity &entity) override;

    void render(double step) override;

    [[nodiscard]] Texture &getTexture() const;
    void setTexture(Texture &texture);

private:
    Texture* m_texture;
};
