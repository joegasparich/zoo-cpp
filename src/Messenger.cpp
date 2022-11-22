#include "util/uuid.h"
#include "Messenger.h"

Messenger::Messenger() : listeners{} {};
Messenger::~Messenger() = default;

std::string Messenger::on(const EventType event, std::function<void(Event* e)> callback) {
    auto handle{ uuid::generate() };

    if (!Messenger::get().listeners.contains(event)) {
        Messenger::get().listeners.insert({event, {}});
    }
    Messenger::get().listeners.at(event).insert_or_assign(handle, std::move(callback));

    return handle;
}

void Messenger::fire(EventType type) {
    fire(Event{type});
}

void Messenger::fire(Event e) {
    fire(&e);
}

void Messenger::fire(Event* e) {
    if (!Messenger::get().listeners.contains(e->type)) return;

    for (const auto& listener : Messenger::get().listeners.at(e->type)) {
        listener.second(e);
    }
}

void Messenger::unsubscribe(const EventType event, const std::string& handle) {
    if (!Messenger::get().listeners.contains(event)) return;
    if (!Messenger::get().listeners.at(event).contains((handle))) return;

    Messenger::get().listeners.at(event).erase(handle);
}
