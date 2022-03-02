#pragma once

template<typename T>
void Entity::addComponent(std::unique_ptr<T> component) {
    // TODO: Check if component already exists in map
    components.put<T>(std::move(component));

    if (hasStarted) {
        component->start(*this);
    }
}

template<typename T>
T *Entity::getComponent(T) const {
    return components.find<T>().get();
}
