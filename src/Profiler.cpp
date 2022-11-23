#include "Profiler.h"
#include "Game.h"

const int PROFILER_UPDATE_FREQ = 100;

Profiler::Profiler() = default;
Profiler::~Profiler() = default;

void Profiler::begin(const std::string& label) {
    auto& instance = Profiler::get();

    if (instance.current != nullptr) {
        TraceLog(LOG_WARNING, "Profiling was not ended");
    }

    instance.rootLabel = label;
    instance.root = std::make_unique<Timer>();
    instance.root->startTime = GetTimeMs();
    instance.root->label = label;

    instance.current = instance.root.get();
}

double Profiler::end(const std::string& label) {
    auto& instance = Profiler::get();

    if (instance.current != instance.root.get()) {
        TraceLog(LOG_WARNING, "Timer was not ended: %s", instance.current->label.c_str());
    }

    instance.root->endTime = GetTimeMs();
    instance.root->time = instance.root->endTime - instance.root->startTime;

    auto time = instance.root->time;

    // TODO: Manually save?
    if (Game::getTicks() % PROFILER_UPDATE_FREQ == 0)
        instance.record[label] = std::move(instance.root);

    instance.current = nullptr;
    instance.root = nullptr;

    return time;
}

void Profiler::startTimer(const std::string& label) {
    auto& instance = Profiler::get();

    auto timer = std::make_unique<Timer>();
    timer->label = label;
    timer->startTime = GetTimeMs();
    timer->parent = instance.current;

    instance.current->children.push_back(std::move(timer));

    instance.current = instance.current->children.back().get();
}

void Profiler::stopTimer(const std::string& label) {
    auto& instance = Profiler::get();

    if (label != instance.current->label) {
        TraceLog(LOG_WARNING, "Incorrect timer ended: %s Current is: %s", label.c_str(), instance.current->label.c_str());
        return;
    }

    instance.current->endTime = GetTimeMs();
    instance.current->time = instance.current->endTime - instance.current->startTime;
    instance.current = instance.current->parent;
}

Timer* Profiler::getRecord(const std::string& label) {
    return Profiler::get().record.at(label).get();
}
