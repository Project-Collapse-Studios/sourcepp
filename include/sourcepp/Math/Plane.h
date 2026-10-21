#pragma once

#include <sourcepp/Math.h>



namespace mathpp {
    class Plane {
        using Point = Vec3f;

        // normal.x * x + normal.y * y + normal.z * z = distance
        Vec3f normal;
        float distance = 0;

    public:
        Plane() = default;

        /// Create a plane using a direction vector, distance and type = 0.
        Plane(const Vec3f& normal, const float distance):
            normal(normal),
            distance(distance) {};

        /// Create a plane using 3 points, distance.
        Plane(const Point& v1, const Point& v2, const Point& v3) {
            this->Plane_From3Points(v1, v2, v3);
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

    protected:
        void Plane_From3Points(const Point& v1, const Point& v2, const Point& v3) {
            auto vec_1 = v2 - v1;
            auto vec_2 = v3 - v1;
            this->normal = vec_1.cross(vec_2); // We don't care about orientation
            this->distance = this->normal.dot(v1);
        }
    };
}