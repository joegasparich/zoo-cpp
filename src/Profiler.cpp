#include "Profiler.h"
#include "Game.h"

const int PROFILER_UPDATE_FREQ = 100;

Profiler::Profiler() = default;
Profiler::~Profiler() = default;

void Profiler::begin(const std::string& label) {
    auto& instance = Profiler::get();

    if (instance.current != nullptr) {
        std::cout << "Profiling was not ended" << std::endl;
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
        std::cout << "Timer was not ended: " << instance.current->label << std::endl;
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
        std::cout << "Incorrect timer ended: " << label << " Current is: " << instance.current->label << std::endl;
        return;
    }

    instance.current->endTime = GetTimeMs();
    instance.current->time = instance.current->endTime - instance.current->startTime;
    instance.current = instance.current->parent;
}

Timer* Profiler::getRecord(const std::string& label) {
    return Profiler::get().record.at(label).get();
}
