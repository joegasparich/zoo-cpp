#include "Mediator.h"

#include <utility>

Mediator * Mediator::instance = nullptr;

Mediator::Mediator() : listeners{} {};
Mediator::~Mediator() noexcept = default;

uuid Mediator::on(const string& event, function<void(string)> callback) {
    auto handle{ random_generator()() };

    if (!listeners.contains(event)) {
        listeners.insert({event, {}});
    }
    listeners.at(event).insert({handle, move(callback)});

    return handle;
}

void Mediator::fire(const string& event, const string& data) {
    if (!listeners.contains(event)) return;

    for (const auto& listener : listeners.at(event)) {
        listener.second(data);
    }
}

void Mediator::unsubscribe(const string& event, uuid handle) {
    if (!listeners.contains(event)) return;
    if (!listeners.at(event).contains((handle))) return;

    listeners.at(event).erase(handle);
}
