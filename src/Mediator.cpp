#include "Mediator.h"

Mediator::Mediator() : listeners{} {};
Mediator::~Mediator() noexcept = default;

uuid Mediator::on(const EventType event, std::function<void(std::string)> callback) {
    auto handle{ random_generator()() };

    if (!Mediator::get().listeners.contains(event)) {
        Mediator::get().listeners.insert({event, {}});
    }
    Mediator::get().listeners.at(event).insert({handle, move(callback)});

    return handle;
}

void Mediator::fire(const EventType event) {
    Mediator::fire(event, "");
}

void Mediator::fire(const EventType event, const std::string& data) {
    std::cout << getEventName(event) << " Event fired with data " << data << std::endl;

    if (!Mediator::get().listeners.contains(event)) return;

    for (const auto& listener : Mediator::get().listeners.at(event)) {
        listener.second(data);
    }
}

void Mediator::unsubscribe(const EventType event, uuid handle) {
    if (!Mediator::get().listeners.contains(event)) return;
    if (!Mediator::get().listeners.at(event).contains((handle))) return;

    Mediator::get().listeners.at(event).erase(handle);
}
