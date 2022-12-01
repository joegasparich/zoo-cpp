#include "Toolbar.h"
#include "ui/GUI.h"
#include "SaveManager.h"
#include "UIManager.h"
#include "DebugPanel.h"
#include "ProfilerPanel.h"
#include "Root.h"

const Rectangle dimensions = {0, 0, SCREEN_WIDTH, 45};
const float BUTTON_WIDTH = 70;

Toolbar::Toolbar(ToolManager* toolManager) : Window(dimensions), toolManager{toolManager} {}

void Toolbar::doWindowContents() {
    Window::doWindowContents();

    float curX = GAP_SMALL;

    GUI::TextAlign = AlignMode::Centre;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Biome"))
        toolManager->setTool(ToolType::Biome);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Elevation"))
        toolManager->setTool(ToolType::Elevation);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Wall"))
        toolManager->setTool(ToolType::Wall);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Door"))
        toolManager->setTool(ToolType::Door);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Path"))
        toolManager->setTool(ToolType::Path);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Object"))
        toolManager->setTool(ToolType::TileObject);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Delete"))
        toolManager->setTool(ToolType::Delete);
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "New"))
        Root::saveManager().newGame();
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Save"))
        Root::saveManager().saveGame("save.json");
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Load"))
        Root::saveManager().loadGame("save.json");
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Debug")) {
        if (!Root::ui().isWindowOpen(debugPanelId)) {
            debugPanelId = Root::ui().pushWindow(std::make_unique<DebugPanel>());
        } else {
            Root::ui().closeWindow(debugPanelId);
        }
    }
    curX += BUTTON_WIDTH + GAP_SMALL;

    if (GUI::buttonText({curX, 10, BUTTON_WIDTH, 25}, "Profiler")) {
        if (!Root::ui().isWindowOpen(profilerPanelId)) {
            profilerPanelId = Root::ui().pushWindow(std::make_unique<ProfilerPanel>());
        } else {
            Root::ui().closeWindow(profilerPanelId);
        }
    }
    curX += BUTTON_WIDTH + GAP_SMALL;

    GUI::TextAlign = AlignMode::TopLeft;
}

