#pragma once

template<typename T>
T* Entity::addComponent(std::unique_ptr<T> component) {
    // TODO: Check if component already exists in map
    components.put<T>(std::move(component));
    auto comp = getComponent<T>();
    baseComponents.put<typename T::base>(comp);

    if (hasStarted) {
        comp->start();
    }

    return comp;
}

template<typename T>
T* Entity::getComponent() const {
    auto it = components.find<T>();
    if (it != components.end()) {
        return static_cast<T*>(it->second.get());
    }
    return static_cast<T*>(baseComponents.find<T>()->second);
}

template<typename T>
bool Entity::hasComponent() const {
    auto it = components.find<T>();
    if (it != components.end()) {
        return baseComponents.find<T>() != baseComponents.end();
    }

    return it != components.end();
}
