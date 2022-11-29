#pragma once

#include "common.h"

struct IRect {
    int x;
    int y;
    int width;
    int height;

    IRect () : x(0), y(0), width(0), height(0) {}
    IRect (int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
    explicit IRect(Rectangle r) : IRect{int(floor(r.x)), int(floor(r.y)), int(floor(r.height)), int(floor(r.width))} {}

    // TODO: Is this fine?
    // TODO: Is the conversion allocating memory and is it slow?
    // implicit conversion
    operator Rectangle() const { return toRectangle(); }

    [[nodiscard]] inline std::string toString() const {
        return std::to_string(x) + "," + std::to_string(y);
    }
    [[nodiscard]] inline Rectangle toRectangle() const {
        return Rectangle{float(x), float(y), float(width), float(height)};
    }

    inline int xMax() { return x + width; }
    inline int yMax() { return y + height; }
};