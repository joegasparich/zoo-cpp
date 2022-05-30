#pragma once

#include "pch.h"
#include <SDL2/SDL.h>

template<typename C, typename P>
bool contains_if(C container, P pred) {
    return std::find_if(
            container.begin(),
            container.end(),
            pred
    ) != container.end();
}

struct SDL_TextureDestroyer {
    void operator()(SDL_Texture *t) const {
        SDL_DestroyTexture(t);
    }
};

struct SDL_RendererDestroyer {
    void operator()(SDL_Renderer *r) const {
        SDL_DestroyRenderer(r);
    }
};

inline std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

inline std::string vecToString(glm::ivec2 v) {
    return std::to_string(v.x) + "," + std::to_string(v.y);
}

inline std::string vecToString(glm::vec2 v) {
    return std::to_string(v.x) + "," + std::to_string(v.y);
}

inline glm::vec2 stringToFVec(std::string s) {
    auto numbers = split(s, ',');
    return {std::stof(numbers[0]), std::stof(numbers[1])};
}
inline glm::ivec2 stringToIVec(std::string s) {
    auto numbers = split(s, ',');
    return {std::stoi(numbers[0]), std::stoi(numbers[1])};
}
