#pragma once

#include <algorithm>

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
