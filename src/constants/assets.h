#pragma once

#include <string>

#define IMG_KEEPER "img/keeper.png"

#define OBJ_BUILDING "objects/building.json"
#define OBJ_TREE "objects/tree.json"

#define WALL_IRON_FENCE "walls/iron_bar_fence.json"
#define WALL_WOODEN_FENCE "walls/wooden_fence.json"

#define PATH_DIRT "paths/dirt_path.json"
#define PATH_COBBLE "paths/cobble_path.json"

namespace assets {
    static const std::string images[] = {
        IMG_KEEPER
    };

    static const std::string objects[] = {
        OBJ_TREE,
        OBJ_BUILDING
    };

    static const std::string walls[] = {
        WALL_IRON_FENCE,
        WALL_WOODEN_FENCE
    };

    static const std::string paths[] = {
        PATH_DIRT,
        PATH_COBBLE
    };
}
