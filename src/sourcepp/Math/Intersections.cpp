#include <sourcepp/Math/Intersections.h>

using Point = sourcepp::math::Vec3f;
using Versor = sourcepp::math::Vec3f;

bool IntersectPlanePlane(const Plane& plane1, const Plane& plane2, ParametricLine& out) {
    const auto& norm1 = plane1.GetNormal();
    const auto& norm2 = plane2.GetNormal();
    auto cross_p = norm1.cross(norm2);

    if (cross_p[0] < FLT_EPSILON && cross_p[1] < FLT_EPSILON && cross_p[2] < FLT_EPSILON) {
        return false;
    }
    // z = 0
    // | n1x n2y | x | = | d1 |
    // | n2x n2y | y | = | d2 |

    const float d1 = plane1.GetDist();
    const float d2 = plane2.GetDist();

    float W = (norm1[0] * norm2[1] - norm2[0] * norm1[1]);

    float x = (d1 * norm2[1] - d2 * norm1[1]) / W;
    float y = (d2 * norm1[0] - d1 * norm2[0]) / W;

    out = ParametricLine(cross_p, Point(x, y, 0));
    return true;
}

bool IntersectPlaneLine(const Plane& plane, const ParametricLine& line, sourcepp::math::Vec3f& out) {
    const auto& plane_dir = plane.GetNormal();
    const auto& line_dir = line.GetDir();
    float d_prod = plane_dir.dot(line_dir);

    if (d_prod < FLT_EPSILON) {
        return false;
    }

    float t = plane.GetDist() - plane_dir.dot(line.GetOrigin()) / d_prod;
    out = line.GetPoint(t);
    return true;
}

bool IntersectPlanePlanePlane(const Plane& plane1, const Plane& plane2, const Plane& plane3, sourcepp::math::Vec3f out) {
    return true;
}