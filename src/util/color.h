#pragma once

#include "common.h"

inline Vector3 col2vec(Color col) {
    return {
        col.r / 255.0f,
        col.g / 255.0f,
        col.b / 255.0f,
    };
}

inline Color vec2Col(Vector3 vec) {
    return {
        u_char(vec.x * 255.0f),
        u_char(vec.y * 255.0f),
        u_char(vec.z * 255.0f),
        255 // a
    };
}

// We use Vector3 here to ensure that values are normalised to a float
inline Vector3 rgb2hsv(Vector3 in) {
    Vector3 out;
    double min, max, delta;

    min = in.x < in.y ? in.x : in.y;
    min = min  < in.z ? min  : in.z;

    max = in.x > in.y ? in.x : in.y;
    max = max  > in.z ? max  : in.z;

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
    if (in.x >= max) { // > is bogus, just keeps compiler happy
        out.x = (in.y - in.z) / delta; // between yellow & magenta
    } else if (in.y >= max) {
        out.x = 2.0 + (in.z - in.x) / delta; // between cyan & yellow
    } else {
        out.x = 4.0 + ( in.x - in.y ) / delta; // between magenta & cyan
    }

    out.x *= 60.0; // degrees
    if( out.x < 0.0 ) {
        out.x += 360.0;
    }

    return out;
}

inline Vector3 hsv2rgb(Vector3 in) {
    double hh, p, q, t, ff;
    long i;
    Vector3 out;

    if (in.x <= 0.0) { // < is bogus, just shuts up warnings
        out.x = in.z;
        out.y = in.z;
        out.z = in.z;
        return out;
    }
    hh = in.x;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = long(hh);
    ff = hh - i;
    p = in.z * (1.0 - in.y);
    q = in.z * (1.0 - (in.y * ff));
    t = in.z * (1.0 - (in.y * (1.0 - ff)));

    switch(i) {
        case 0:
            out.x = in.z;
            out.y = t;
            out.z = p;
            break;
        case 1:
            out.x = q;
            out.y = in.z;
            out.z = p;
            break;
        case 2:
            out.x = p;
            out.y = in.z;
            out.z = t;
            break;
        case 3:
            out.x = p;
            out.y = q;
            out.z = in.z;
            break;
        case 4:
            out.x = t;
            out.y = p;
            out.z = in.z;
            break;
        case 5:
        default:
            out.x = in.z;
            out.y = p;
            out.z = q;
            break;
    }
    return out;
}
