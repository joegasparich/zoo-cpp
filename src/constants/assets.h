#pragma once

#include <string>

#define IMG_KEEPER "assets/img/keeper.png"

#define OBJ_BUILDING "assets/objects/building.json"
#define OBJ_TREE "assets/objects/tree.json"

#define WALL_IRON_FENCE "assets/walls/iron_bar_fence.json"

#define PATH_DIRT "assets/paths/dirt_path.json"

namespace assets {
    static const char* images[] = {
        IMG_KEEPER
    };

    static const char* objects[] = {
        OBJ_TREE,
        OBJ_BUILDING
    };

    static const char* walls[] = {
        WALL_IRON_FENCE
    };

    static const char* paths[] = {
        PATH_DIRT
    };
}
