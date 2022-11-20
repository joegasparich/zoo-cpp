#include "BiomeTool.h"
#include "ToolManager.h"
#include "ZooScene.h"
#include "UIManager.h"
#include "ui/GUI.h"

const float DEFAULT_RADIUS = 0.65f;
const float MARGIN = GAP_SMALL;
const float BUTTON_SIZE = 30;

BiomeTool::BiomeTool(ToolManager &toolManager) : Tool(toolManager) {}
BiomeTool::~BiomeTool() = default;

void BiomeTool::set() {
    currentBiome = Biome::Sand;
    toolManager.ghost->type = GhostType::Circle;
    toolManager.ghost->radius = DEFAULT_RADIUS;
    toolManager.ghost->elevate = true;
}

void BiomeTool::onInput(InputEvent* event) {
    // Only listen to down and up events so that we can't start dragging from UI
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) dragging = true;
    if (event->mouseButtonUp == MOUSE_BUTTON_LEFT) dragging = false;

    // TODO (optimisation): Don't fire every tick
    if (dragging) {
        Root::zoo()->world->biomeGrid->setBiomeInRadius(Root::renderer().screenToWorldPos(GetMousePosition()), DEFAULT_RADIUS, currentBiome);
    }
}

void BiomeTool::onGUI() {
    Root::ui().doImmediateWindow("immBiomePanel", {10, 60, 200, BUTTON_SIZE + MARGIN * 2}, [&](auto rect) {
        for(auto i = 0; i < NUM_BIOMES; i++) {
            auto biome = (Biome)i;
            auto [name, colour] = getBiomeInfo(biome);

            // TODO: Wrap
            Rectangle buttonRect = {i * (BUTTON_SIZE + GAP_SMALL) + MARGIN, MARGIN, BUTTON_SIZE, BUTTON_SIZE};

            GUI::drawRect(buttonRect, colour);
            GUI::highlightMouseover(buttonRect);

            if (currentBiome == biome)
                GUI::drawBorder(buttonRect, 2, BLACK);

            if (GUI::clickableArea(buttonRect))
                setBiome(biome);
        }
    });
}

std::string BiomeTool::getName() {
    return "Biome Tool";
}

ToolType BiomeTool::getType() {
    return ToolType::Biome;
}

Biome BiomeTool::getBiome() {
    return currentBiome;
}

void BiomeTool::setBiome(Biome biome) {
    currentBiome = biome;
}
