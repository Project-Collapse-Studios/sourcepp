#pragma once

#include <sourcepp/Math.h>



namespace mathpp {
    class Plane {
        using Point = Vec3f;

        // normal.x * x + normal.y * y + normal.z * z = distance
        Vec3f normal;
        float distance = 0;
        int type = 0;

    public:
        /// Create a plane using a direction vector, distance and type = 0.
        Plane(const Vec3f& normal, const float distance, const int type = 0):
            normal(normal),
            distance(distance),
            type(type) {};

        /// Create a plane using 3 points, distance and type = 0.
        Plane(const Point& v1, const Point& v2, const Point& v3, const int type = 0):
            type(type) {
            auto vec_1 = v2 - v1;
            auto vec_2 = v3 - v1;
            this->normal = vec_1.cross(vec_2); // We don't care about orientation
            this->distance = this->normal.dot(v1);
        };

        [[nodiscard]] Vec3f GetNormal() const {
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
}