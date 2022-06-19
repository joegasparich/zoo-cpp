#include "common.h"

void glm::to_json(json &j, const glm::vec2 &P) {
    j = { { "x", P.x }, { "y", P.y } };
}

void glm::from_json(const json &j, glm::vec2 &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
}

void glm::to_json(json &j, const glm::vec3 &P) {
    j = { { "x", P.x }, { "y", P.y }, { "z", P.z } };
}

void glm::from_json(const json &j, glm::vec3 &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
    P.z = j.at("z").get<double>();
}

void glm::to_json(json &j, const glm::vec4 &P) {
    j = { { "x", P.x }, { "y", P.y }, { "z", P.z }, { "w", P.w } };
}

void glm::from_json(const json &j, glm::vec4 &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
    P.z = j.at("z").get<double>();
    P.w = j.at("w").get<double>();
}

void glm::to_json(json &j, const glm::ivec2 &P) {
    j = { { "x", P.x }, { "y", P.y } };
}

void glm::from_json(const json &j, glm::ivec2 &P) {
    P.x = j.at("x").get<int>();
    P.y = j.at("y").get<int>();
}

void glm::to_json(json &j, const glm::ivec3 &P) {
    j = { { "x", P.x }, { "y", P.y }, { "z", P.z } };
}

void glm::from_json(const json &j, glm::ivec3 &P) {
    P.x = j.at("x").get<int>();
    P.y = j.at("y").get<int>();
    P.z = j.at("z").get<int>();
}

void glm::to_json(json &j, const glm::ivec4 &P) {
    j = { { "x", P.x }, { "y", P.y }, { "z", P.z }, { "w", P.w } };
}

void glm::from_json(const json &j, glm::ivec4 &P) {
    P.x = j.at("x").get<int>();
    P.y = j.at("y").get<int>();
    P.z = j.at("z").get<int>();
    P.w = j.at("w").get<int>();
}
