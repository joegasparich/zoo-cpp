#pragma once

#include "common.h"

inline json readJSON(std::string filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();

    auto j = json::parse(buffer.str());

    file.close();

    return j;
}

//inline Vector2 fromJSON(json j) {
//    return Vector2{
//        j["x"].get<float>(),
//        j["y"].get<float>()
//    };
//}

// Serialisation
inline void to_json(json &j, const Cell &P) {
    j = { { "x", P.x }, { "y", P.y } };
}

inline void from_json(const json &j, Cell &P) {
    P.x = j.at("x").get<long>();
    P.y = j.at("y").get<long>();
}

inline void to_json(json &j, const Vector2 &P) {
    j = { { "x", P.x }, { "y", P.y } };
}

inline void from_json(const json &j, Vector2 &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
}

inline void to_json(json &j, const Vector3 &P) {
    j = { { "x", P.x }, { "y", P.y }, { "z", P.z } };
}

inline void from_json(const json &j, Vector3 &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
    P.z = j.at("z").get<double>();
}

inline void to_json(json &j, const Vector4 &P) {
    j = { { "x", P.x }, { "y", P.y }, { "z", P.z }, { "w", P.w } };
}

inline void from_json(const json &j, Vector4 &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
    P.z = j.at("z").get<double>();
    P.w = j.at("w").get<double>();
}

inline void to_json(json &j, const Color &P) {
    j = { { "r", P.r }, { "g", P.g }, { "b", P.b }, { "a", P.a } };
}

inline void from_json(const json &j, Color &P) {
    P.r = j.at("r").get<char>();
    P.g = j.at("g").get<char>();
    P.b = j.at("b").get<char>();
    P.a = j.at("a").get<char>();
}

inline void to_json(json &j, const Rectangle &P) {
    j = { { "x", P.x }, { "y", P.y }, { "w", P.width }, { "h", P.height } };
}

inline void from_json(const json &j, Rectangle &P) {
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
    P.width = j.at("w").get<double>();
    P.height = j.at("h").get<double>();
}