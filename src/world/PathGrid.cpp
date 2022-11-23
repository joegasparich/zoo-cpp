#include "PathGrid.h"
#include "ZooScene.h"
#include "constants/assets.h"
#include "Messenger.h"
#include "constants/world.h"
#include "constants/depth.h"

PathGrid::PathGrid(unsigned int rows, unsigned int cols) : rows(rows), cols(cols) {}

void PathGrid::setup() {
    // Grid
    for (unsigned int i = 0; i < cols; i++) {
        grid.emplace_back();
        for (unsigned int j = 0; j < rows; j++) {
            grid.at(i).push_back({
                nullptr,
                {i, j},
                false,
                false
            });
        }
    }

    auto paths = Registry::getAllPaths();

    isSetup = true;
}

void PathGrid::cleanup() {
    grid.clear();
    cols = 0;
    rows = 0;

    isSetup = false;
}

void PathGrid::render() {
    // Maybe use GenMeshCustom?
    // Test performance on this

    for (unsigned int i = 0; i < cols; i++) {
        for (unsigned int j = 0; j < rows; j++) {
            auto& path = grid.at(i).at(j);
            if (path.exists) {
                auto [spriteIndex, elevation] = PathGrid::getSpriteInfo(path);
                auto pos = Vector2{float(i), float(j)};
                pos -= Vector2{0.0f, 1.0f + elevation}; // Offset cell size
                auto spritesheet = Root::assetManager().getSpriteSheet(path.data->spriteSheetPath);

                BlitOptions opts;
                opts.texture = spritesheet->texture;
                opts.source = spritesheet->getCellBounds(spriteIndex);
                opts.depth = DEPTH::GROUND;
                opts.pos = pos * WORLD_SCALE;
                opts.scale = Vector2{1, 2} * WORLD_SCALE;

                Root::renderer().blit(opts);
            }
        }
    }
}

Path *PathGrid::placePathAtTile(PathData *data, Cell tilePos) {
    if (!isSetup) return nullptr;
    if (!Root::zoo()->world->isPositionInMap(tilePos)) return nullptr;
    if (getPathAtTile(tilePos)->exists) return nullptr;

    grid.at(tilePos.x).at(tilePos.y) = Path{
        data,
        tilePos,
        true,
        false
    };

    // TODO: update pathfinding

    return &grid.at(tilePos.x).at(tilePos.y);
}

Path *PathGrid::getPathAtTile(Cell tilePos) {
    if (!Root::zoo()->world->isPositionInMap(tilePos)) return nullptr;

    return &grid.at(tilePos.x).at(tilePos.y);
}

PathSpriteInfo PathGrid::getSpriteInfo(Path &path) {
    // TODO: is it less efficient to convert to vector instead of an overload
    auto nw = Root::zoo()->world->elevationGrid->getElevationAtPos(path.pos);
    auto ne = Root::zoo()->world->elevationGrid->getElevationAtPos(Cell{path.pos.x + 1, path.pos.y});
    auto sw = Root::zoo()->world->elevationGrid->getElevationAtPos(Cell{path.pos.x, path.pos.y + 1});
    auto se = Root::zoo()->world->elevationGrid->getElevationAtPos(Cell{path.pos.x + 1, path.pos.y + 1});
    auto elevation = std::min({nw, ne, sw, se});

    if (nw == ne && nw == sw && nw == se) return {(unsigned int)PathSpriteIndex::Flat, elevation};
    if (nw == ne && nw > sw && sw == se) return {(unsigned int)PathSpriteIndex::HillNorth, elevation};
    if (nw == ne && nw < sw && sw == se) return {(unsigned int)PathSpriteIndex::HillSouth, elevation};
    if (nw == sw && nw < ne && ne == se) return {(unsigned int)PathSpriteIndex::HillEast, elevation};
    if (nw == sw && nw > ne && ne == se) return {(unsigned int)PathSpriteIndex::HillWest, elevation};

    return {(unsigned int)PathSpriteIndex::Flat, 0.0f};
}

void PathGrid::serialise() {
    if (Root::saveManager().mode == SerialiseMode::Loading)
        cleanup();

    Root::saveManager().SerialiseValue("cols", cols);
    Root::saveManager().SerialiseValue("rows", rows);

    std::function<json()> get = [&](){
        std::vector<std::vector<json>> gridData;
        std::transform(grid.begin(), grid.end(), std::back_inserter(gridData), [](std::vector<Path>& row) {
            std::vector<json> rowData;
            std::transform(row.begin(), row.end(), std::back_inserter(rowData), [](Path& path) {
                return json({
                    {"assetPath", path.exists ? path.data->assetPath : ""},
                    {"indestructable", path.indestructable}
                });
            });
            return rowData;
        });

        return gridData;
    };
    std::function<void(json)> set = [&](json data){
        auto gridData = data.get<std::vector<std::vector<json>>>();

        for (int i = 0; i < cols; i++) {
            grid.emplace_back();
            for (int j = 0; j < rows; j++) {
                auto pathData = gridData.at(i).at(j);
                auto assetPath = pathData.at("assetPath").get<std::string>();
                auto exists = assetPath.length() > 0;

                grid.at(i).push_back({
                     exists ? &Registry::getPath(pathData.at("assetPath").get<std::string>()) : nullptr,
                     {i, j},
                     exists,
                     false
                 });
            }
        }
    };
    Root::saveManager().SerialiseValue("grid", get, set);

    if (Root::saveManager().mode == SerialiseMode::Loading)
        isSetup = true;
}
