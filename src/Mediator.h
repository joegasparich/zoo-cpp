#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <vector>
#include <map>
#include <functional>

using namespace boost::uuids;

enum class EventType {
    ApplicationStarted, ApplicationLoaded, ApplicationEnding,
    WindowClosed, WindowResized, WindowFocused, WindowBlurred, WindowMoved,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    InputPressed, InputReleased
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
        default: return "Unknown Event";
    }
}

class Mediator {
public:
    static Mediator &get() {
        static Mediator instance;
        return instance;
    }

    static uuid on(const EventType event, std::function<void(std::string)>);
    static void fire(const EventType event);
    static void fire(const EventType event, const std::string& data);
    static void unsubscribe(const EventType event, uuid handle);

private:
    Mediator();
    ~Mediator() noexcept;

    std::map<EventType, std::map<uuid, std::function<void(std::string)>>> listeners;
};
