#pragma once

#include "common.h"

struct Cell {
    long x;
    long y;

    Cell() : x(0), y(0) {}
    Cell(long x, long y) : x(x), y(y) {}
    explicit Cell(Vector2 v) : Cell{long(floor(v.x)), long(floor(v.y))} {}

    // TODO: Is this fine?
    // TODO: Is the conversion allocating memory and is it slow?
    // implicit conversion
    operator Vector2() const { return toVector(); }

    [[nodiscard]] inline std::string toString() const {
        return std::to_string(x) + "," + std::to_string(y);
    }
    [[nodiscard]] inline Vector2 toVector() const {
        return Vector2{float(x), float(y)};
    }
};

namespace std {
    template<>
    struct hash<Cell> {
        inline size_t operator()(const Cell& c) const {
            // size_t value = your hash computations over x
            auto A = (c.x >= 0 ? 2 * c.x : -2 * c.x - 1);
            auto B = (c.y >= 0 ? 2 * c.y : -2 * c.y - 1);
            auto C = ((A >= B ? A * A + A + B : A + B * B) / 2);
            return c.x < 0 && c.y < 0 || c.x >= 0 && c.y >= 0 ? C : -C - 1;
        }
    };
}

inline bool operator == (Cell const& lhs, Cell const& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

inline Cell operator+(const Cell& a, const Cell& b) {
    return {a.x + b.x, a.y + b.y};
}

inline Cell operator-(const Cell& a, const Cell& b) {
    return {a.x - b.x, a.y - b.y};
}
