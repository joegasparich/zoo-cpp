#pragma once

#include "pch.h"

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
    // Serialisation
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

    // Structured bindings
    template <std::size_t I, auto N, class T, auto Q>
    constexpr auto& get(glm::vec<N, T, Q>& v) noexcept { return v[I]; }

    template <std::size_t I, auto N, class T, auto Q>
    constexpr const auto& get(const glm::vec<N, T, Q>& v) noexcept { return v[I]; }

    template <std::size_t I, auto N, class T, auto Q>
    constexpr auto&& get(glm::vec<N, T, Q>&& v) noexcept { return std::move(v[I]); }

    template <std::size_t I, auto N, class T, auto Q>
    constexpr const auto&& get(const glm::vec<N, T, Q>&& v) noexcept { return std::move(v[I]); }
}

namespace std {
    // Structured bindings
    template <auto N, class T, auto Q>
    struct tuple_size<glm::vec<N, T, Q>> : std::integral_constant<std::size_t, N> { };

    template <std::size_t I, auto N, class T, auto Q>
    struct tuple_element<I, glm::vec<N, T, Q>> {
        using type = decltype(get<I>(declval<glm::vec<N,T,Q>>()));
    };
}