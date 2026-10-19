#pragma once

#include <sourcepp/Math.h>

class ParametricLine {
    // Point = Versor * t + Point
    using Versor = sourcepp::math::Vec3f;
    using Point = sourcepp::math::Vec3f;

    Versor dir;
    Point origin;

public:
    ParametricLine(const sourcepp::math::Vec3f& dir_, const Point& origin_):
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