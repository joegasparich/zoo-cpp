## Template

[Component.h](Component.h)
```cpp
enum COMPONENT {
    ...
    NEW_COMPONENT
};
```
Header
```cpp
#pragma once

#include "common.h"
#include "Component.h"

class NewComponent : public Component {
public:
    COMPONENT getId() override;

    NewComponent(Entity *entity);
};
```
Source
```cpp
#include "NewComponent.h"
#include "entities/Entity.h"

COMPONENT NewComponent::getId() { return NEW_COMPONENT; }

NewComponent::NewComponent(Entity *entity) : Component(entity) {}
```
[createComponentById.cpp](createComponentById.cpp)
```cpp
        case NEW_COMPONENT:
            entity->addComponent(std::make_unique<NewComponent>(entity));
            return entity->getComponent<NewComponent>();
```