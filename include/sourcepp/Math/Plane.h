#pragma once

#include <sourcepp/Math.h>
#include <sourcepp/Math/ParametricLine.h>
#include <sourcepp/Math/Intersections.h> // This is actually used!

class Plane {
    using Versor = sourcepp::math::Vec3f;
    using Point = sourcepp::math::Vec3f;

    // normal.x * x + normal.y * y + normal.z * z = distance
    Versor normal;
    float distance = 0;
    int type = 0;

public:
    Plane(const sourcepp::math::Vec3f& normal, float distance, int type):
        normal(normal),
        distance(distance),
        type(type) {
        this->normal.norm();
    };

    Plane(const Point& v1, const Point& v2, const Point& v3, float distance, int type):
        distance(distance),
        type(type) {
        auto vec_1 = v2 - v1;
        auto vec_2 = v3 - v1;
        this->normal = vec_1.cross(vec_2); // We don't care about orientation
        this->normal.norm();
    };

    [[nodiscard]] Versor GetNormal() const {
        return this->normal;
    }

    [[nodiscard]] float GetDist() const {
        return this->distance;
    }


    // Intersections
    bool Intersect(const Plane& other, ParametricLine& out) const {
        return IntersectPlanePlane(*this, other, out);
    }

    ParametricLine operator&(const Plane& other) const {
        ParametricLine out(Versor{}, Point{});
        IntersectPlanePlane(*this, other, out);
        return out;
    }

    bool Intersect(const ParametricLine& line, Point& out) const {
        return IntersectPlaneLine(*this, line, out);
    }

    Point operator&(const ParametricLine& other) const {
        Point out{};
        IntersectPlaneLine(*this, other, out);
        return out;
    }
};

