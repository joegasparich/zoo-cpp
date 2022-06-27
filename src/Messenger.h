#pragma once

#include "pch.h"

enum class EventType {
    ApplicationStarted, ApplicationLoaded, ApplicationEnding,
    WindowClosed, WindowResized, WindowFocused, WindowBlurred, WindowMoved,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    InputPressed, InputReleased,
    ElevationUpdated
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
        case EventType::KeyPressed: return "Key Pressed";
        case EventType::KeyReleased: return "Key Released";
        case EventType::MouseButtonPressed: return "Mouse Button Pressed";
        case EventType::MouseButtonReleased: return "Mouse Button Released";
        case EventType::MouseMoved: return "Mouse Moved";
        case EventType::MouseScrolled: return "Mouse Scrolled";
        case EventType::InputPressed: return "Input Pressed";
        case EventType::InputReleased: return "Input Released";
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
    ~Messenger() noexcept;

    std::map<EventType, std::map<std::string, std::function<void(json& data)>>> listeners;
};
