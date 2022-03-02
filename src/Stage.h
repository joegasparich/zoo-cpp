#pragma once

#include <vector>

#include "Entity.h"

class Stage {
public:
    Stage();

    void setup();
    void update();
    void render(double step) const;
private:
    std::vector<std::unique_ptr<Entity>> entities;
};
