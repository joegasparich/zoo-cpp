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

namespace glm {
    void to_json(json& j, const glm::vec2& P);
    void from_json(const json& j, glm::vec2& P);
    void to_json(json& j, const glm::vec3& P);
    void from_json(const json& j, glm::vec3& P);
    void to_json(json& j, const glm::vec4& P);
    void from_json(const json& j, glm::vec4& P);
    void to_json(json& j, const glm::ivec2& P);
    void from_json(const json& j, glm::ivec2& P);
    void to_json(json& j, const glm::ivec3& P);
    void from_json(const json& j, glm::ivec3& P);
    void to_json(json& j, const glm::ivec4& P);
    void from_json(const json& j, glm::ivec4& P);
}
