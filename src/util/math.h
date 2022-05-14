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

inline glm::vec3 rgb2hsv(glm::vec3 in) {
    glm::vec3 out;
    double min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.z = max;
    delta = max - min;
    if (delta < 0.00001) {
        out.y = 0;
        out.x = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.y = (delta / max);
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.y = 0.0;
        out.x = NAN; // its now undefined
        return out;
    }
    if (in.r >= max) { // > is bogus, just keeps compiler happy
        out.x = (in.g - in.b) / delta; // between yellow & magenta
    } else if (in.g >= max) {
        out.x = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
    } else {
        out.x = 4.0 + ( in.r - in.g ) / delta; // between magenta & cyan
    }

    out.x *= 60.0; // degrees
    if( out.x < 0.0 ) {
        out.x += 360.0;
    }

    return out;
}

inline glm::vec3 hsv2rgb(glm::vec3 in) {
    double hh, p, q, t, ff;
    long i;
    glm::vec3 out;

    if (in.s <= 0.0) { // < is bogus, just shuts up warnings
        out.r = in.z;
        out.g = in.z;
        out.b = in.z;
        return out;
    }
    hh = in.x;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.z * (1.0 - in.y);
    q = in.z * (1.0 - (in.y * ff));
    t = in.z * (1.0 - (in.y * (1.0 - ff)));

    switch(i) {
        case 0:
            out.r = in.z;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in.z;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in.z;
            out.b = t;
            break;
        case 3:
            out.r = p;
            out.g = q;
            out.b = in.z;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in.z;
            break;
        case 5:
        default:
            out.r = in.z;
            out.g = p;
            out.b = q;
            break;
    }
    return out;
}
