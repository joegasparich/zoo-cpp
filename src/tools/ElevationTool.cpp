#include "ElevationTool.h"
#include "ToolManager.h"
#include "Game.h"
#include "ZooScene.h"
#include "UIManager.h"
#include "ui/GUI.h"

ElevationTool::ElevationTool(ToolManager &toolManager) : Tool(toolManager) {}
ElevationTool::~ElevationTool() = default;

void ElevationTool::set() {
    currentElevation = Elevation::Hill;
    toolManager.ghost->type = GhostType::Circle;
    toolManager.ghost->elevate = true;
}

void ElevationTool::onInput(InputEvent* event) {
    // Only listen to down and up events so that we can't start dragging from UI
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        dragging = true;
        oldElevationData = std::make_unique<json>();
        Root::saveManager().setCurrentSerialiseNode(oldElevationData.get());
        Root::saveManager().mode = SerialiseMode::Saving;
        Root::saveManager().SerialiseDeep("elevation", Root::zoo()->world->elevationGrid.get());

        event->consume();
    }
    if (event->mouseButtonUp == MOUSE_BUTTON_LEFT) {
        if (!dragging) return;
        dragging = false;

        toolManager.pushAction(std::make_unique<Action>(Action{
            "Elevation brush",
            *oldElevationData,
            [] (json& data) {
                Root::saveManager().setCurrentSerialiseNode(&data);
                Root::saveManager().mode = SerialiseMode::Loading;
                Root::saveManager().SerialiseDeep("elevation", Root::zoo()->world->elevationGrid.get());
            }
        }));

        event->consume();
    }
}

void ElevationTool::update() {
    if (dragging && Root::game().getTicks() % 5 == 0) {
        Root::zoo()->world->elevationGrid->setElevationInCircle(Root::input().getMouseWorldPos(), 1.0f, currentElevation);
    }
}

void ElevationTool::onGUI() {
    Root::ui().doImmediateWindow("immElevationPanel", {10, 60, 100, 100}, [&](auto rect) {
        GUI::TextAlign = AlignMode::Centre;

        if (GUI::buttonText({ 10, 10, 80, 20}, "Water")) currentElevation = Elevation::Water;
        if (GUI::buttonText({ 10, 40, 80, 20}, "Flat")) currentElevation = Elevation::Flat;
        if (GUI::buttonText({ 10, 70, 80, 20}, "Hill")) currentElevation = Elevation::Hill;

        GUI::TextAlign = AlignMode::TopLeft;
    });
}

std::string ElevationTool::getName() {
    return "Elevation Tool";
}

ToolType ElevationTool::getType() {
    return ToolType::Elevation;
}

Elevation ElevationTool::getCurrentElevation() const {
    return currentElevation;
}

void ElevationTool::setCurrentElevation(Elevation currentElevation) {
    currentElevation = currentElevation;
}
