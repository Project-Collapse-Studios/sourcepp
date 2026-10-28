#pragma once

#include "Vec.h"

namespace mathpp {
    class ParametricLine {

        Vec3f dir;
        Point origin;

    public:
        ParametricLine(const Vec3f& dir_, const Point& origin_):
            dir(dir_),
            origin(origin_) {}

        ParametricLine() = default;

        /// Get the direction versor of this line.
        [[nodiscard]] const Vec3f& GetDir() const {
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