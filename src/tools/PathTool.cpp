#include "PathTool.h"
#include "ToolManager.h"
#include "constants/assets.h"
#include "world/PathGrid.h"
#include "ZooScene.h"
#include "Game.h"
#include "UIManager.h"
#include "ui/GUI.h"

const float MARGIN = GAP_SMALL;
const float BUTTON_SIZE = 30;

PathTool::PathTool(ToolManager &toolManager) :
    Tool(toolManager),
    allPaths{Registry::getAllPaths()} {}
PathTool::~PathTool() = default;

void PathTool::set() {
    toolManager.ghost->type = GhostType::SpriteSheet;
    toolManager.ghost->snap = true;
    toolManager.ghost->scale = {1.0f, 2.0f};
    toolManager.ghost->offset = {0.0f, -1.0f};

    // Temp
    setPath(&Registry::getPath(PATH_DIRT));
}

void PathTool::onInput(InputEvent* event) {
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        isDragging = true;
        dragTile = flr(Root::renderer().screenToWorldPos(event->mousePos));
    }

    if (event->mouseButtonUp == MOUSE_BUTTON_LEFT) {
        isDragging = false;

        while (ghosts.size() > 0) {
            auto& ghost = ghosts.back();
            if (ghost->canPlace) {
                Root::zoo()->world->pathGrid->placePathAtTile(currentPath, flr(ghost->pos));
            }
            ghosts.pop_back();
        }
    }
}

void PathTool::update() {
    if (!currentPath) return;

    auto mousePos = Root::renderer().screenToWorldPos(GetMousePosition());

    updateGhostSprite(*toolManager.ghost);

    if (isDragging) {
        // Dragging
        toolManager.ghost->visible = false;

        auto xDif = flr(mousePos).x - dragTile.x;
        auto yDif = flr(mousePos).y - dragTile.y;
        auto horizontal = abs(xDif) > abs(yDif);
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
            updateGhostSprite(*ghost);

            if (horizontal) {
                i += sign(flr(mousePos).x - i);
            } else {
                j += sign(flr(mousePos).y - j);
            }
        }

        // Pop additional ghosts
        while (ghosts.size() > length) {
            ghosts.pop_back();
        }
    } else {
        toolManager.ghost->visible = true;

        updateGhostSprite(*toolManager.ghost);
    }
}

void PathTool::onGUI() {
    Root::ui().doImmediateWindow("immPathPanel", {10, 60, 200, BUTTON_SIZE + MARGIN * 2}, [&](auto rect) {
        for(auto i = 0; i < allPaths.size(); i++) {
            auto path = allPaths[i];

            // TODO: Wrap
            Rectangle buttonRect = {i * (BUTTON_SIZE + GAP_SMALL) + MARGIN, MARGIN, BUTTON_SIZE, BUTTON_SIZE};

            auto spritesheet = Root::assetManager().getSpriteSheet(path->spriteSheetPath);
            GUI::drawSubTexture(buttonRect, spritesheet->texture, bottomHalf(spritesheet->getCellBounds(0)));

            GUI::highlightMouseover(buttonRect);

            if (currentPath == path)
                GUI::drawBorder(buttonRect, 2, BLACK);

            if (GUI::clickableArea(buttonRect))
                setPath(path);
        }
    });
}

void PathTool::setPath(PathData *path) {
    currentPath = path;

    toolManager.ghost->spriteSheet = Root::assetManager().getSpriteSheet(currentPath->spriteSheetPath);
}

PathData *PathTool::getPath() {
    return currentPath;
}

void PathTool::render() {
    for (auto& ghost : ghosts) {
        ghost->render();
    }
}

void PathTool::updateGhostSprite(ToolGhost& ghost) {
    auto path = Root::zoo()->world->pathGrid->getPathAtTile(Cell{ghost.pos});

    if (!path) {
        ghost.visible = false;
        ghost.canPlace = false;
        return;
    }

    auto [spriteIndex, elevation] = PathGrid::getSpriteInfo(*path);

    ghost.spriteSheetIndex = spriteIndex;

    ghost.visible = true;
    ghost.canPlace = true;
    if (path->exists) ghost.canPlace = false;
    if (Root::zoo()->world->elevationGrid->isPositionSlopeCorner(path->pos)) ghost.canPlace = false;
    if (Root::zoo()->world->elevationGrid->isPositionWater(path->pos)) ghost.canPlace = false;

    ghost.offset = {0.0f, -1.0f - elevation};
}

std::string PathTool::getName() {
    return "Path Tool";
}

ToolType PathTool::getType() {
    return ToolType::Path;
}