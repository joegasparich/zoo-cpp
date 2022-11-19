#include "ProfilerPanel.h"
#include "ui/GUI.h"
#include "Profiler.h"

const Rectangle dimensions = {0, 70, 200, 200};

ProfilerPanel::ProfilerPanel() : Window(dimensions) {}

float drawTimer(Rectangle r, Timer* timer, int depth, float curY) {
    Rectangle textRect = {r.x + depth * 10, curY, r.width - depth * 4, 12};
    GUI::drawText(textRect, timer->label + ": " + std::to_string(timer->time) + "ms");
    curY += 12;

    for (auto& child : timer->children) {
        curY = drawTimer(r, child.get(), depth + 1, curY);
    }

    return curY;
}

void ProfilerPanel::doWindowContents(Rectangle r) {
    Window::doWindowContents(r);

    float curY = 0;
    curY = drawTimer(r, Profiler::getRecord("tick"), 0, curY);
    curY = drawTimer(r, Profiler::getRecord("render"), 0, curY);
}