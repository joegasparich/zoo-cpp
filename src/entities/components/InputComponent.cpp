#include "InputComponent.h"
#include "entities/Entity.h"

InputComponent::InputComponent(Entity *entity) : Component(entity) {}

COMPONENT InputComponent::getId() { return INPUT_COMPONENT; }
