#pragma once

#include "Component.h"

class TileObjectComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    TileObjectComponent(const ObjectData &data);

    void start(Entity &entity) override;

    ObjectData m_data;
private:
};
