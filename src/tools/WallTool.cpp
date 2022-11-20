#include "ZooScene.h"
#include "constants/assets.h"
#include "WallTool.h"
#include "ToolManager.h"
#include "UIManager.h"
#include "ui/GUI.h"

const float MARGIN = GAP_SMALL;
const float BUTTON_SIZE = 30;

WallTool::WallTool(ToolManager &toolManager) :
    Tool(toolManager),
    allWalls{Registry::getAllWalls()} {}
WallTool::~WallTool() = default;

void WallTool::set() {
    currentWall = nullptr;
    toolManager.ghost->type = GhostType::SpriteSheet;
    toolManager.ghost->snap = true;
    toolManager.ghost->scale = {1.0f, 2.0f};
    toolManager.ghost->offset = {0.0f, -1.0f};

    // Temp
    setWall(&Registry::getWall(WALL_IRON_FENCE));
}

void WallTool::onInput(InputEvent* event) {
    auto worldMousePos = Root::renderer().screenToWorldPos(event->mousePos);

    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        isDragging = true;
        dragTile = floor(worldMousePos);
        dragQuadrant = World::getQuadrantAtPos(worldMousePos);
    }

    if (event->mouseButtonUp == MOUSE_BUTTON_LEFT) {
        isDragging = false;

        // Reverse so we are going from drag start to drag end
        std::reverse(ghosts.begin(), ghosts.end());

        while (ghosts.size() > 0) {
            auto& ghost = ghosts.back();
            if (ghost->canPlace) {
                Root::zoo()->world->wallGrid->placeWallAtTile(currentWall, floor(ghost->pos), dragQuadrant);
            }
            ghosts.pop_back();
        }
    }
}

void WallTool::update() {
    if (!currentWall) return;

    auto& wallGrid = Root::zoo()->world->wallGrid;

    auto mousePos = Root::renderer().screenToWorldPos(GetMousePosition());
    auto mouseQuadrant = World::getQuadrantAtPos(mousePos);

    if (isDragging) {
        // Dragging
        toolManager.ghost->visible = false;

        auto xDif = floor(mousePos).x - dragTile.x;
        auto yDif = floor(mousePos).y - dragTile.y;
        auto horizontal = dragQuadrant == Side::North || dragQuadrant == Side::South;
        auto length = (horizontal ? abs(xDif) : abs(yDif)) + 1;

        // Push new ghosts to reach length
        while (ghosts.size() < length) {
            auto ghost = std::make_unique<ToolGhost>();
            ghost->type = GhostType::SpriteSheet;
            ghost->follow = false;
            ghost->spriteSheet = toolManager.ghost->spriteSheet;
            ghost->offset = toolManager.ghost->offset;
            ghost->scale = toolManager.ghost->scale;
            ghosts.push_back(std::move(ghost));
        }

        auto i = floor(dragTile.x);
        auto j = floor(dragTile.y);
        for (auto& ghost : ghosts) {
            ghost->pos = {float(i), float(j)};
            updateGhostSprite(*ghost, {long(i), long(j)}, dragQuadrant);

            if (horizontal) {
                i += sign(floor(mousePos).x - i);
            } else {
                j += sign(floor(mousePos).y - j);
            }
        }

        // Pop additional ghosts
        while (ghosts.size() > length) {
            ghosts.pop_back();
        }
    } else {
        toolManager.ghost->visible = true;

        updateGhostSprite(*toolManager.ghost, floor(mousePos), mouseQuadrant);
    }
}

void WallTool::render() {
    for (auto& ghost : ghosts) {
        ghost->render();
    }
}

void WallTool::onGUI() {
    Root::ui().doImmediateWindow("immWallPanel", {10, 60, 200, BUTTON_SIZE + MARGIN * 2}, [&](auto rect) {
        for(auto i = 0; i < allWalls.size(); i++) {
            auto path = allWalls[i];

            // TODO: Wrap
            Rectangle buttonRect = {i * (BUTTON_SIZE + GAP_SMALL) + MARGIN, MARGIN, BUTTON_SIZE, BUTTON_SIZE};

            auto spritesheet = Root::assetManager().getSpriteSheet(path->spriteSheetPath);
            GUI::drawSubTexture(buttonRect, spritesheet->texture, bottomPct(spritesheet->getCellBounds(0), 0.25f));

            GUI::highlightMouseover(buttonRect);

            if (currentWall == path)
                GUI::drawBorder(buttonRect, 2, BLACK);

            if (GUI::clickableArea(buttonRect))
                setWall(path);
        }
    });
}

void WallTool::updateGhostSprite(ToolGhost& ghost, Cell tilePos, Side quadrant) {
    auto wall = Root::zoo()->world->wallGrid->getWallAtTile(tilePos, quadrant);
    auto& world = Root::zoo()->world;
    auto& elevationGrid = world->elevationGrid;

    if (!wall) {
        ghost.visible = false;
        return;
    }
    ghost.visible = true;

    ghost.canPlace = true;
    if (wall->exists) ghost.canPlace = false;
    auto [v1, v2] = world->wallGrid->getWallVertices(*wall);
    if (elevationGrid->getElevationAtPos(v1) < 0) ghost.canPlace = false;
    if (elevationGrid->getElevationAtPos(v2) < 0) ghost.canPlace = false;
    auto tiles = world->wallGrid->getAdjacentTiles(*wall);

    Entity* blockingObj;
    for (auto tile : tiles) {
        auto obj = world->getTileObjectAtPosition(tile);
        if (!blockingObj && obj) blockingObj = obj;

        if (obj && obj == blockingObj) ghost.canPlace = false;
    }

    auto [spriteIndex, elevation] = WallGrid::getSpriteInfo(*wall);

    ghost.spriteSheetIndex = spriteIndex;

    switch (quadrant) {
        case Side::North:
            ghost.offset = {0.0f, -2.0f - elevation};
            break;
        case Side::South:
            ghost.offset = {0.0f, -1.0f - elevation};
            break;
        case Side::West:
            ghost.offset = {-0.5f, -1.0f - elevation};
            break;
        case Side::East:
            ghost.offset = {0.5f, -1.0f - elevation};
            break;
        default:
            break;
    }
}

void WallTool::setWall(WallData* wall) {
    currentWall = wall;

    toolManager.ghost->spriteSheet = Root::assetManager().getSpriteSheet(wall->spriteSheetPath);
}

WallData &WallTool::getWall() {
    return *currentWall;
}

std::string WallTool::getName() {
    return "Wall Tool";
}

ToolType WallTool::getType() {
    return ToolType::Wall;
}
