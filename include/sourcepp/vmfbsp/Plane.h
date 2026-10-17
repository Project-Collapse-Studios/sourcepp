#pragma once

#include <sourcepp/Math.h>

using Vertex = sourcepp::math::Vec3f;

class Plane {
    // normal.x * x + normal.y * y + normal.z * z = distance
    sourcepp::math::Vec3f normal;
    float distance = 0;
    int type = 0;

public:
    Plane(const sourcepp::math::Vec3f& normal, float distance, int type):
        normal(normal),
        distance(distance),
        type(type)
        {};

    Plane(const Vertex& v1, const Vertex& v2, const Vertex& v3, float distance, int type):
        distance(distance),
        type(type) {
        auto vec_1 = v2 - v1;
        auto vec_2 = v3 - v1;
        this->normal = vec_1.cross(vec_2); // We don't care about orientation
        this->normal.norm();
    };
};

