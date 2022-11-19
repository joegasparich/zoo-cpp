#pragma once

template<typename T>
T* Entity::addComponent(std::unique_ptr<T> component) {
    // TODO: Check if component already exists in map
    components.put<T>(std::move(component));
    auto comp = getComponent<T>();

    if (hasStarted) {
        comp->start();
    }

    return comp;
}

template<typename T>
T* Entity::getComponent() const {
    auto it = components.find<T>();
    return static_cast<T*>(it->second.get());
}

template<typename T>
bool Entity::hasComponent() const {
    auto it = components.find<T>();
    return it != components.end();
}
