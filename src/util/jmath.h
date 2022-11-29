#pragma once

#include "common.h"
#include <time.h>

// Random
inline float randomFloat() {
    return float(rand())/float(RAND_MAX);
}

inline unsigned char randomChar() {
    return (unsigned char)(random() * CHAR_MAX);
}

// General math
inline int sign(float val) {
    if (val == 0) return 0;
    return val > 0 ? 1 : -1;
}

template<typename T>
inline T clamp(T val, T minVal, T maxVal) {
    return std::min(std::max(val, minVal), maxVal);
}

inline float normalise(float val, float min, float max) {
    return (val - min) / (max - min);
}

inline float lerp(float from, float to, float pct) {
    return from + (to - from) * pct;
}

inline int flr(float a) {
    return int(floor(a));
}

inline int clg(float a) {
    return int(ceil(a));
}

// Extended min max (stolen from glm)
template<typename T>
inline T min(T const& x, T const& y) {
    return std::min(x, y);
}

template<typename T>
inline T min(T const& x, T const& y, T const& z) {
    return std::min(std::min(x, y), z);
}

template<typename T, template<typename> class C>
inline C<T> min(C<T> const& x, typename C<T>::T const& y, typename C<T>::T const& z) {
    return std::min(std::min(x, y), z);
}

template<typename T, template<typename> class C>
inline C<T> min (C<T> const& x, C<T> const& y, C<T> const& z) {
    return std::min(std::min(x, y), z);
}

template<typename T>
inline T min(T const& x, T const& y, T const& z, T const& w) {
    return std::min(std::min(x, y), std::min(z, w));
}

template<typename T, template<typename> class C>
inline C<T> min(C<T> const& x, typename C<T>::T const& y, typename C<T>::T const& z, typename C<T>::T const& w) {
    return std::min(std::min(x, y), std::min(z, w));
}

template<typename T, template<typename> class C>
inline C<T> min(C<T> const& x, C<T> const& y, C<T> const& z, C<T> const& w) {
    return std::min(std::min(x, y), std::min(z, w));
}

template<typename T>
inline T max(T const& x, T const& y) {
    return std::max(x, y);
}

template<typename T>
inline T max(T const& x, T const& y, T const& z) {
    return std::max(std::max(x, y), z);
}

template<typename T, template<typename> class C>
inline C<T> max(C<T> const& x, typename C<T>::T const& y, typename C<T>::T const& z) {
    return std::max(std::max(x, y), z);
}

template<typename T, template<typename> class C>
inline C<T> max(C<T> const& x, C<T> const& y, C<T> const& z) {
    return std::max(std::max(x, y), z);
}

template<typename T>
inline T max(T const& x, T const& y, T const& z, T const& w) {
    return std::max(std::max(x, y), std::max(z, w));
}

template<typename T, template<typename> class C>
inline C<T> max(C<T> const& x, typename C<T>::T const& y, typename C<T>::T const& z, typename C<T>::T const& w) {
    return std::max(std::max(x, y), std::max(z, w));
}

template<typename T, template<typename> class C>
inline C<T> max(C<T> const& x, C<T> const& y, C<T> const& z, C<T> const& w) {
    return std::max(std::max(x, y), std::max(z, w));
}

// Collision
inline bool pointInCircle(Vector2 circlePos, float radius, Vector2 point) {
    auto dx = circlePos.x - point.x;
    auto dy = circlePos.y - point.y;
    return dx * dx + dy * dy < radius * radius;
}

inline bool pointInRect(Rectangle rect, Vector2 point) {
    return point.x >= rect.x && point.x <= rect.x + rect.width
        && point.y >= rect.y && point.y <= rect.y + rect.height;
}

inline bool circleIntersectsRect(Vector2 boxPos, Vector2 boxDim, Vector2 circlePos, float circleRad) {
    auto distX = abs(circlePos.x - boxPos.x - boxDim.x / 2);
    auto distY = abs(circlePos.y - boxPos.y - boxDim.y / 2);

    if (distX > boxDim.x / 2 + circleRad) {
        return false;
    }
    if (distY > boxDim.y / 2 + circleRad) {
        return false;
    }

    if (distX <= boxDim.x / 2) {
        return true;
    }
    if (distY <= boxDim.y / 2) {
        return true;
    }

    auto dx = distX - boxDim.x / 2;
    auto dy = distY - boxDim.y / 2;
    return dx * dx + dy * dy <= circleRad * circleRad;
}

// Vectors
inline Vector2 multVect(Vector2 a, Vector2 b) {
    return {
        a.x * b.x,
        a.y * b.y
    };
}

inline Vector2 divVect(Vector2 a, Vector2 b) {
    return {
        a.x / b.x,
        a.y / b.y
    };
}

inline Cell round(Vector2 a) {
    return {
        int(std::round(a.x)),
        int(std::round(a.y))
    };
}

inline Cell flr(Vector2 a) {
    return {
        int(std::floor(a.x)),
        int(std::floor(a.y))
    };
}
inline Cell clg(Vector2 a) {
    return {
        int(std::ceil(a.x)),
        int(std::ceil(a.y))
    };
}

// Rectangles
inline Rectangle atZero(Rectangle r) {
    return { 0, 0, r.width, r.height};
}
inline Rectangle contract(Rectangle r, float amt) {
    return { r.x + amt, r.y + amt, r.width - amt * 2, r.height - amt * 2 };
}
inline Rectangle expand(Rectangle r, float amt) {
    return { r.x - amt, r.y - amt, r.width + amt * 2, r.height + amt * 2 };
}
inline Rectangle topPct(Rectangle r, float pct) {
    return { r.x, r.y, r.width, r.height * pct };
}
inline Rectangle bottomPct(Rectangle r, float pct) {
    return { r.x, r.y + r.height * (1 - pct), r.width, r.height * pct };
}
inline Rectangle leftPct(Rectangle r, float pct) {
    return { r.x, r.y, r.width * pct, r.height };
}
inline Rectangle rightPct(Rectangle r, float pct) {
    return { r.x + r.width * (1 - pct), r.y, r.width * pct, r.height };
}
inline Rectangle topHalf(Rectangle r) {
    return topPct(r, 0.5f);
}
inline Rectangle bottomHalf(Rectangle r) {
    return bottomPct(r, 0.5f);
}
inline Rectangle leftHalf(Rectangle r) {
    return leftPct(r, 0.5f);
}
inline Rectangle rightHalf(Rectangle r) {
    return rightPct(r, 0.5f);
}