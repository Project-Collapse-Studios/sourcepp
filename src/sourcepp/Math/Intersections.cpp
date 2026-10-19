#include <sourcepp/Math/Intersections.h>

using namespace mathpp;
using Point = Vec3f;

bool Intersect(const Plane& plane1, const Plane& plane2, ParametricLine& out) {
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

    const float W = (norm1[0] * norm2[1] - norm2[0] * norm1[1]);

    const float x = (d1 * norm2[1] - d2 * norm1[1]) / W;
    const float y = (d2 * norm1[0] - d1 * norm2[0]) / W;

    out = ParametricLine(cross_p, Point(x, y, 0));
    return true;
}

bool Intersect(const Plane& plane, const ParametricLine& line, Point& out) {
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

bool Intersect(const Plane& plane1, const Plane& plane2, const Plane& plane3, Point& out) {
    /* | A1 B1 C1 | x | = | D1 |
     * | A2 B2 C2 | y | = | D2 |
     * | A3 B3 C3 | z | = | D3 |
     *
     */
    Mat3x3f matrix;
    const auto& n1 = plane1.GetNormal();
    const auto& n2 = plane2.GetNormal();
    const auto& n3 = plane3.GetNormal();

    // Column 1
    matrix(0, 0) = n1[0];
    matrix(1, 0) = n2[0];
    matrix(2, 0) = n3[0];

    // Column 2
    matrix(0, 1) = n1[1];
    matrix(1, 1) = n2[1];
    matrix(2, 1) = n3[1];

    // Column 3
    matrix(0, 2) = n1[2];
    matrix(1, 2) = n2[2];
    matrix(2, 2) = n3[2];

    const float W = matrix.det();
    if (W < FLT_EPSILON) { // System of equations cannot be solved (2 or more planes are not intersecting).
        return false;
    }

    auto ReplaceColumnAndDet = [&matrix, &n1, &n2, &n3, &plane1, &plane2, &plane3](const uint8_t row_id) -> float {
        matrix(0, row_id) = plane1.GetDist();
        matrix(1, row_id) = plane2.GetDist();
        matrix(2, row_id) = plane3.GetDist();
        const float det = matrix.det();
        matrix(0, row_id) = n1[row_id];
        matrix(1, row_id) = n2[row_id];
        matrix(2, row_id) = n3[row_id];
        return det;
    };

    const float Wx = ReplaceColumnAndDet(0);
    const float Wy = ReplaceColumnAndDet(1);
    const float Wz = ReplaceColumnAndDet(2);

    out = Point(Wx/W, Wy/W, Wz/W);
    return true;
}