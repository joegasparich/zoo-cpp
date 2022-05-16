#pragma once

inline float lerp(float start, float end, float percent) {
    return start + (end - start) * percent;
}

inline bool pointInCircle(glm::vec2 circlePos, float radius, glm::vec2 point) {
    auto dx = circlePos.x - point.x;
    auto dy = circlePos.y - point.y;
    return dx * dx + dy * dy < radius * radius;
}

inline bool circleIntersectsRect(glm::vec2 boxPos, glm::vec2 boxDim, glm::vec2 circlePos, float circleRad) {
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

inline glm::vec2 multVect(glm::vec2 a, glm::vec2 b) {
    return {
        a.x * b.x,
        a.y * b.y
    };
}

inline glm::vec2 divVect(glm::vec2 a, glm::vec2 b) {
    return {
        a.x / b.x,
        a.y / b.y
    };
}
