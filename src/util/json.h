#pragma once

#include "pch.h"

inline json readJSON(std::string filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();

    auto j = json::parse(buffer.str());

    file.close();

    return j;
}

glm::vec2 fromJSON(json j) {
    return glm::vec2{
        j["x"].get<float>(),
        j["y"].get<float>()
    };
}
