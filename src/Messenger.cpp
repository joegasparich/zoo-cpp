#include <util/uuid.h>
#include "Messenger.h"

Messenger::Messenger() : listeners{} {};
Messenger::~Messenger() noexcept = default;

std::string Messenger::on(const EventType event, std::function<void(json& data)> callback) {
    auto handle{ uuid::generate() };

    if (!Messenger::get().listeners.contains(event)) {
        Messenger::get().listeners.insert({event, {}});
    }
    Messenger::get().listeners.at(event).insert_or_assign(handle, move(callback));

    return handle;
}

void Messenger::fire(const EventType event) {
    Messenger::fire(event, {});
}

void Messenger::fire(const EventType event, json&& data) {
    if (!Messenger::get().listeners.contains(event)) return;

    for (const auto& listener : Messenger::get().listeners.at(event)) {
        listener.second(data);
    }
}

void Messenger::unsubscribe(const EventType event, std::string handle) {
    if (!Messenger::get().listeners.contains(event)) return;
    if (!Messenger::get().listeners.at(event).contains((handle))) return;

    Messenger::get().listeners.at(event).erase(handle);
}
