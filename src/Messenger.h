#pragma once

#include "common.h"

enum class EventType {
    ApplicationStarted, ApplicationLoaded, ApplicationEnding,
    WindowClosed, WindowResized, WindowFocused, WindowBlurred, WindowMoved,
    InputEvent,
    ElevationUpdated, AreasUpdated
};

constexpr const char* getEventName(EventType type) {
    switch (type) {
        case EventType::ApplicationStarted: return "Application Started";
        case EventType::ApplicationLoaded: return "Application Loaded";
        case EventType::ApplicationEnding: return "Application Ending";
        case EventType::WindowClosed: return "Window Closed";
        case EventType::WindowResized: return "Window Resized";
        case EventType::WindowFocused: return "Window Focused";
        case EventType::WindowBlurred: return "Window Blurred";
        case EventType::WindowMoved: return "Window Moved";
        case EventType::InputEvent: return "Input Event";
        case EventType::ElevationUpdated: return "Elevation Updated";
        default: return "Unknown Event";
    }
}

class Messenger {
public:
    static Messenger &get() {
        static Messenger instance;
        return instance;
    }

    static std::string on(const EventType event, std::function<void(json& data)>);
    static void fire(const EventType event);
    static void fire(const EventType event, json&& data);
    static void unsubscribe(const EventType event, std::string handle);

private:
    Messenger();
    ~Messenger();

    std::map<EventType, std::map<std::string, std::function<void(json& data)>>> listeners;
};
