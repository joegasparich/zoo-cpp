#include "constants/assets.h"
#include "entities/entityGenerators.h"
#include "TileObjectTool.h"
#include "ToolManager.h"
#include "UIManager.h"
#include "ui/GUI.h"
#include "constants/colours.h"

const float MARGIN = GAP_SMALL;
const float BUTTON_SIZE = 30;

TileObjectTool::TileObjectTool(ToolManager &toolManager) :
    Tool(toolManager),
    allObjects{Registry::getAllObjects()} {}
TileObjectTool::~TileObjectTool() = default;

void TileObjectTool::set() {
    toolManager.ghost->type = GhostType::Sprite;
    toolManager.ghost->snap = true;
    toolManager.ghost->scale = {1.0f, 1.0f};
    toolManager.ghost->offset = {0.5f, 0.5f};
    toolManager.ghost->elevate = true;

    // Temp
    setObject(&Registry::getObject(OBJ_BUILDING));
}

void TileObjectTool::onInput(InputEvent* event) {
    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        if (!toolManager.ghost->canPlace) return;

        auto object = createTileObject(currentObject->assetPath, toolManager.ghost->pos);

        if (!object) return;

        Root::zoo()->world->registerTileObject(object.get());
        auto id = Root::zoo()->registerEntity(std::move(object));

        toolManager.pushAction(std::make_unique<Action>(Action{
            "Place " + currentObject->name,
            id,
            [] (json& data) {
                Root::zoo()->getEntityById(data.get<unsigned int>())->destroy();
            }
        }));

        event->consume();
    }
}

void TileObjectTool::update() {
    if (!currentObject) return;
    auto& elevationGrid = Root::zoo()->world->elevationGrid;

    toolManager.ghost->visible = true;
    toolManager.ghost->canPlace = true;

    for (int i = 0; i < currentObject->size.x; i++) {
        for (int j = 0; j < currentObject->size.y; j++) {
            auto pos = flr(InputManager::getMouseWorldPos()) + Cell{i, j};
            if (Root::zoo()->world->getTileObjectAtPosition(pos)) toolManager.ghost->canPlace = false;
            if (elevationGrid->isTileWater(pos)) toolManager.ghost->canPlace = false;
            if (!Root::zoo()->world->isPositionInMap(pos)) {
                toolManager.ghost->visible = false;
                return;
            }
        }
    }
}

void TileObjectTool::onGUI() {
    Root::ui().doImmediateWindow("immObjectPanel", {10, 60, 200, BUTTON_SIZE + MARGIN * 2}, [&](auto rect) {
        for(auto i = 0; i < allObjects.size(); i++) {
            auto object = allObjects[i];

            // TODO: Wrap
            Rectangle buttonRect = {i * (BUTTON_SIZE + GAP_SMALL) + MARGIN, MARGIN, BUTTON_SIZE, BUTTON_SIZE};

            GUI::drawRect(buttonRect, UI_BUTTON_COLOUR);

            if (!object->spritePath.empty())
                GUI::drawTexture(contract(buttonRect, 2), Root::assetManager().getTexture(object->spritePath));
            else if (!object->spriteSheetPath.empty()) {
                auto spritesheet = Root::assetManager().getSpriteSheet(object->spriteSheetPath);
                GUI::drawSubTexture(contract(buttonRect, 2), spritesheet->texture, spritesheet->getCellBounds(0));
            }

            GUI::highlightMouseover(buttonRect);

            if (currentObject == object)
                GUI::drawBorder(buttonRect, 2, BLACK);

            if (GUI::clickableArea(buttonRect))
                setObject(object);
        }
    });
}

void TileObjectTool::setObject(ObjectData* object) {
    currentObject = object;

    if (!object->spritePath.empty()) {
        toolManager.ghost->type = GhostType::Sprite;
        toolManager.ghost->sprite = Root::assetManager().getTexture(object->spritePath);
    } else if (!object->spriteSheetPath.empty()) {
        toolManager.ghost->type = GhostType::SpriteSheet;
        toolManager.ghost->spriteSheet = Root::assetManager().getSpriteSheet(object->spriteSheetPath);
        toolManager.ghost->spriteSheetIndex = 0;
    } else {
        TraceLog(LOG_WARNING, "[TileObjectTool] No object sprite: %s", object->name.c_str());
    }
    toolManager.ghost->pivot = object->pivot;
    toolManager.ghost->offset = object->size / 2.0f;
}

ObjectData* TileObjectTool::getObject() {
    return currentObject ? currentObject : nullptr;
}

std::string TileObjectTool::getName() {
    return "Object Tool";
}

ToolType TileObjectTool::getType() {
    return ToolType::TileObject;
}
