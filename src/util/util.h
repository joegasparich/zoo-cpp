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

inline std::string vecToString(glm::vec2 v) {
    return std::to_string(v.x) + "," + std::to_string(v.y);
}

inline glm::vec2 stringToVec(std::string s) {
    return glm::vec2((int)s[0], (int)s[2]);
}
