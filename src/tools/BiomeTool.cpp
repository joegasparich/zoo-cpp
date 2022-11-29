#include "BiomeTool.h"
#include "ToolManager.h"
#include "ZooScene.h"
#include "UIManager.h"
#include "ui/GUI.h"

const float DEFAULT_RADIUS = 0.65f;
const float MARGIN = GAP_SMALL;
const float BUTTON_SIZE = 30;

BiomeTool::BiomeTool(ToolManager &toolManager) : Tool(toolManager) {
    oldChunkData = std::make_unique<std::map<std::string, json>>();
}
BiomeTool::~BiomeTool() = default;

void BiomeTool::set() {
    currentBiome = Biome::Sand;
    toolManager.ghost->type = GhostType::Circle;
    toolManager.ghost->radius = DEFAULT_RADIUS;
    toolManager.ghost->elevate = true;
}

void BiomeTool::update() {
    if (dragging && Root::game().getTicks() % 5 == 0) {
        // Save backups for undo
        for (auto chunk : Root::zoo()->world->biomeGrid->getChunksInRadius(InputManager::getMouseWorldPos() * BIOME_SCALE, DEFAULT_RADIUS * BIOME_SCALE)) {
            auto cellKey = Cell{int(chunk->x), int(chunk->y)}.toString();
            if (!oldChunkData->contains(cellKey))
                oldChunkData->insert({cellKey, chunk->save()});
        }

        Root::zoo()->world->biomeGrid->setBiomeInRadius(InputManager::getMouseWorldPos() * BIOME_SCALE, DEFAULT_RADIUS * BIOME_SCALE, currentBiome);
    }
}

void BiomeTool::onInput(InputEvent* event) {
    // Only listen to down and up events so that we can't start dragging from UI
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        dragging = true;
        event->consume();
    }
    if (event->mouseButtonUp == MOUSE_BUTTON_LEFT) {
        if (!dragging) return;
        dragging = false;

        toolManager.pushAction(std::make_unique<Action>(Action{
            "Biome brush",
            *oldChunkData,
            [] (json& data) {
                auto chunksData = data.get<std::map<std::string, json>>();
                for(auto [posStr, chunkData] : chunksData) {
                    auto pos = cellFromString(posStr);
                    auto chunk = Root::zoo()->world->biomeGrid->getChunk(pos.x, pos.y);
                    chunk->load(chunkData.get<std::vector<std::vector<std::vector<Biome>>>>());
                }
            }
        }));

        oldChunkData->clear();
        event->consume();
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
