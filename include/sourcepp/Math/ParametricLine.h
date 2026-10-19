#pragma once

#include <sourcepp/Math.h>

namespace mathpp {
    class ParametricLine {
        // Point = Versor * t + Point
        using Versor = Vec3f;
        using Point = Vec3f;

        Versor dir;
        Point origin;

    public:
        ParametricLine(const Vec3f& dir_, const Point& origin_):
            dir(dir_),
            origin(origin_) {
            this->dir.norm();
        }

        ParametricLine() = default;

        /// Get the direction versor of this line.
        [[nodiscard]] const Versor& GetDir() const {
            return this->dir;
        }

        /// Get the point belonging to this line.
        [[nodiscard]] const Point& GetOrigin() const {
            return this->origin;
        }

        [[nodiscard]] Point GetPoint(const float t) const {
            return this->dir * t + this->origin;
        }

        friend std::ostream& operator<<(std::ostream& op, const ParametricLine& line) {
            op << "<PLine|Dir: " << line.dir << "|Point: " << line.origin << ">";
            return op;
        }
    };
}