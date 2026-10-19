#pragma once

#include <sourcepp/Math.h>

class Plane {
    using Versor = sourcepp::math::Vec3f;
    using Point = sourcepp::math::Vec3f;

    // normal.x * x + normal.y * y + normal.z * z = distance
    Versor normal;
    float distance = 0;
    int type = 0;

public:
    /// Create a plane using a direction vector, distance and type = 0.
    /// Direction vector gets normalized to a versor, distance gets accounted for.
    Plane(const sourcepp::math::Vec3f& normal, const float distance, const int type = 0):
        normal(normal),
        distance(distance),
        type(type) {
        this->distance /= this->normal.norm(); // If we normalize the vector we also need to account for the distance being different
    };

    /// Create a plane using 3 points, distance and type = 0.
    /// The distance between those points doesn't matter, only their location as the direction vector gets normalized.
    Plane(const Point& v1, const Point& v2, const Point& v3, const float distance, const int type = 0):
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



    friend std::ostream& operator<<(std::ostream& op, const Plane& plane) {
        op << "<Plane|Normal: " << plane.normal << "|Distance: " << plane.distance << ">";
        return op;
    }
};

