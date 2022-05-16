#pragma once

#include <SDL2/SDL.h>
#include <renderer/SubTexture.h>
#include "pch.h"

#include "Component.h"
#include "renderer/Texture.h"
#include "util/util.h"

class RenderComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    explicit RenderComponent(Entity *entity, Texture *texture);
    explicit RenderComponent(Entity *entity, std::unique_ptr<SubTexture> subTexture);

    void start() override;
    void render(double step) override;

    [[nodiscard]] Texture &getTexture() const;
    void setTexture(Texture &texture);
    void setSubTexture(std::unique_ptr<SubTexture> subTexture);

    glm::vec2 m_pivot = glm::vec2{0.5f, 0.5f};
private:
    Texture* m_texture;
    std::unique_ptr<SubTexture> m_subTexture;
};
