#pragma once

#include "common.h"

struct Timer {
    std::string label;
    double startTime;
    double endTime;
    double time;
    Timer* parent;
    std::vector<std::unique_ptr<Timer>> children;
};

class Profiler {
public:
    Profiler(const Profiler &) = delete;

    static Profiler &get() {
        static Profiler instance;
        return instance;
    }

    static void begin(const std::string& label);
    static double end(const std::string& label);
    static void startTimer(const std::string& label);
    static void stopTimer(const std::string& label);

    static Timer* getRecord(const std::string& label);
private:
    Profiler();
    ~Profiler();

    std::string rootLabel;
    std::unique_ptr<Timer> root;
    Timer* current;
    std::map<std::string, std::unique_ptr<Timer>> record;
};
