#pragma once

template<typename T>
T* Entity::addComponent(std::unique_ptr<T> component) {
    // TODO: Check if component already exists in map
    m_components.put<T>(std::move(component));
    auto comp = getComponent<T>();

    if (m_hasStarted) {
        comp->start(*this);
    }

    return comp;
}

template<typename T>
T* Entity::getComponent() const {
    auto it = m_components.find<T>();
    return static_cast<T*>(it->second.get());
}

template<typename T>
bool Entity::hasComponent() const {
    auto it = m_components.find<T>();
    return it != m_components.end();
}
