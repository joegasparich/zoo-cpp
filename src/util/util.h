#pragma once

#include "common.h"

template<typename C, typename P>
bool contains_if(C container, P pred) {
    return std::find_if(
            container.begin(),
            container.end(),
            pred
    ) != container.end();
}

inline std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}
