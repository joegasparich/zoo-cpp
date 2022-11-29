#include "DebugPanel.h"
#include "ui/GUI.h"
#include "Renderer.h"
#include "Root.h"

const float BUTTON_WIDTH = 100;
const float BUTTON_HEIGHT = 25;
const Color ENABLED = {26, 110, 20, 255};
const Color DISABLED = {130, 26, 26, 255};
const float PANEL_WIDTH = (BUTTON_WIDTH + GAP_SMALL * 2);
const Rectangle dimensions = {SCREEN_WIDTH - PANEL_WIDTH, 70, PANEL_WIDTH, 200};

DebugPanel::DebugPanel() : Window(dimensions) {}

void DebugPanel::doWindowContents(Rectangle rect) {
    Window::doWindowContents(rect);

    float curY = 10;

    GUI::TextColour = WHITE;
    GUI::TextAlign = AlignMode::Centre;

    if (GUI::buttonText({10, curY, BUTTON_WIDTH, BUTTON_HEIGHT}, "Cell Grid", Root::zoo()->debugSettings.cellGrid ? ENABLED : DISABLED))
        Root::zoo()->debugSettings.cellGrid = !Root::zoo()->debugSettings.cellGrid;
    curY += BUTTON_HEIGHT + GAP_SMALL;

    if (GUI::buttonText({10, curY, BUTTON_WIDTH, BUTTON_HEIGHT}, "Biome Chunks", Root::zoo()->debugSettings.biomeChunks ? ENABLED : DISABLED))
        Root::zoo()->debugSettings.biomeChunks = !Root::zoo()->debugSettings.biomeChunks;
    curY += BUTTON_HEIGHT + GAP_SMALL;

    if (GUI::buttonText({10, curY, BUTTON_WIDTH, BUTTON_HEIGHT}, "Elevation Grid", Root::zoo()->debugSettings.elevationGrid ? ENABLED : DISABLED))
        Root::zoo()->debugSettings.elevationGrid = !Root::zoo()->debugSettings.elevationGrid;
    curY += BUTTON_HEIGHT + GAP_SMALL;

    if (GUI::buttonText({10, curY, BUTTON_WIDTH, BUTTON_HEIGHT}, "Area Grid", Root::zoo()->debugSettings.areaGrid ? ENABLED : DISABLED))
        Root::zoo()->debugSettings.areaGrid = !Root::zoo()->debugSettings.areaGrid;
    curY += BUTTON_HEIGHT + GAP_SMALL;

    if (GUI::buttonText({10, curY, BUTTON_WIDTH, BUTTON_HEIGHT}, "Pathfinding Grid", Root::zoo()->debugSettings.pathfindingGrid ? ENABLED : DISABLED))
        Root::zoo()->debugSettings.pathfindingGrid = !Root::zoo()->debugSettings.pathfindingGrid;
    curY += BUTTON_HEIGHT + GAP_SMALL;

    GUI::TextColour = BLACK;
    GUI::TextAlign = AlignMode::TopLeft;
}
