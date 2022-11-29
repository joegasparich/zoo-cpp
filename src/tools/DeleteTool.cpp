#include "DeleteTool.h"
#include "ToolManager.h"
#include "ToolGhost.h"
#include "Root.h"
#include "entities/components/RenderComponent.h"

DeleteTool::DeleteTool (ToolManager& toolManager) : Tool(toolManager) {}
DeleteTool::~DeleteTool () = default;

void DeleteTool::set () {
    toolManager.ghost->type = GhostType::Square;
    toolManager.ghost->snap = true;
    toolManager.ghost->elevate = true;
    toolManager.ghost->ghostColour = BLOCKED_COLOUR;
}

void DeleteTool::update () {
    toolManager.ghost->canPlace = true;

    if (isDragging) {
        auto dragRect = getDragRect();

        toolManager.ghost->follow = false;
        toolManager.ghost->pos = Cell{dragRect.x, dragRect.y};
        toolManager.ghost->scale = Cell{dragRect.width, dragRect.height};

        for (auto t : getHighlightedTileObjects())
            t->getComponent<RenderComponent>()->overrideColour = toolManager.ghost->ghostColour;
        for (auto tile : getHighlightedPaths())
            Root::zoo()->world->pathGrid->getPathAtTile(tile)->overrideColour = toolManager.ghost->ghostColour;
        for (auto wall : getHighlightedWalls())
            wall->overrideColour = toolManager.ghost->ghostColour;
    }
}

void DeleteTool::onInput (InputEvent* event) {
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        dragStartTile = flr(event->mouseWorldPos);
        isDragging = true;
        event->consume();
    }

    if (isDragging && event->mouseButtonUp == MOUSE_BUTTON_LEFT) {
        auto tileObjects = getHighlightedTileObjects();

        json undoData{};
        Root::saveManager().setCurrentSerialiseNode(&undoData);
        Root::saveManager().mode = SerialiseMode::Saving;
        Root::saveManager().SerialiseCollectionDeep("tileObjects", tileObjects);
        Root::saveManager().SerialiseDeep("paths", Root::zoo()->world->pathGrid.get());
        Root::saveManager().SerialiseDeep("walls", Root::zoo()->world->wallGrid.get());

        // Delete tile objects
        for (Entity* t : tileObjects)
            t->destroy();
        // Delete paths
        for (Cell tile : getHighlightedPaths())
            Root::zoo()->world->pathGrid->removePathAtTile(tile);
        // Delete walls
        for (Wall* wall : getHighlightedWalls())
            Root::zoo()->world->wallGrid->deleteWall(wall);

        toolManager.pushAction(std::make_unique<Action>(Action{
            "Delete",
            undoData,
            [] (json& data) {
                Root::saveManager().mode = SerialiseMode::Loading;
                Root::saveManager().setCurrentSerialiseNode(&data);
                Root::zoo()->loadEntitiesFromJson(data.at("tileObjects"));
                Root::saveManager().SerialiseDeep("paths", Root::zoo()->world->pathGrid.get());
                Root::saveManager().SerialiseDeep("walls", Root::zoo()->world->wallGrid.get());
            }
        }));

        toolManager.ghost->follow = true;
        toolManager.ghost->scale = Cell(1, 1);

        isDragging = false;
        event->consume();
    }
}

IRect DeleteTool::getDragRect() {
    auto dragEndTile = flr(InputManager::getMouseWorldPos());
    auto upperLeft = Cell(min(dragStartTile.x, dragEndTile.x), min(dragStartTile.y, dragEndTile.y));
    auto bottomRight = Cell(max(dragStartTile.x, dragEndTile.x), max(dragStartTile.y, dragEndTile.y));
    auto width = bottomRight.x - upperLeft.x + 1;
    auto height = bottomRight.y - upperLeft.y + 1;

    return { upperLeft.x, upperLeft.y, width, height };
}

std::vector<Entity*> DeleteTool::getHighlightedTileObjects () {
    std::vector<Entity*> tileObjects{};
    auto dragRect = getDragRect();

    for (int i = dragRect.x; i < dragRect.xMax(); i++) {
        for (int j = dragRect.y; j < dragRect.yMax(); j++) {
            auto tileObject = Root::zoo()->world->getTileObjectAtPosition(Cell{i, j});
            if (tileObject) tileObjects.push_back(tileObject);
        }
    }

    return tileObjects;
}

std::vector<Cell> DeleteTool::getHighlightedPaths () {
    std::vector<Cell> paths{};
    auto dragRect = getDragRect();

    for (int i = dragRect.x; i < dragRect.xMax(); i++) {
        for (int j = dragRect.y; j < dragRect.yMax(); j++) {
            if (Root::zoo()->world->pathGrid->getPathAtTile(Cell{i, j})->exists)
                paths.emplace_back(i, j);
        }
    }

    return paths;
}

std::vector<Wall*> DeleteTool::getHighlightedWalls () {
    std::vector<Wall*> walls{};
    auto dragRect = getDragRect();

    for (int i = dragRect.x; i < dragRect.xMax(); i++) {
        for (int j = dragRect.y; j < dragRect.yMax(); j++) {
            Cell cell{i, j};
            for (auto wall : Root::zoo()->world->wallGrid->getSurroundingWalls(cell)) {
                if (!wall->exists) continue;

                auto opposite = Root::zoo()->world->wallGrid->getOppositeTile(*wall, cell);
                if (pointInRect(IRect{dragRect.x, dragRect.y, dragRect.width - 1, dragRect.height - 1}, opposite)) {
                    walls.push_back(wall);
                }
            }
        }
    }

    return walls;
}

std::string DeleteTool::getName () {
    return "Delete Tool";
}

ToolType DeleteTool::getType () {
    return ToolType::Delete;
}