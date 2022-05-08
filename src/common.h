#pragma once

enum class Side {
    North = 0,
    East = 1,
    South = 2,
    West = 3
};

inline void logVec(glm::vec2 vec) {
    std::cout << vec.x << ", " << vec.y << std::endl;
}
