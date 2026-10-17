#pragma once


class Plane;
class ParametricLine;
bool IntersectPlanePlane(const Plane& plane1, const Plane& plane2, ParametricLine& out);
bool IntersectPlaneLine(const Plane& plane, const ParametricLine& line, sourcepp::math::Vec3f& out);
bool IntersectPlanePlanePlane(const Plane& plane1, const Plane& plane2, const Plane& plane3, sourcepp::math::Vec3f out);

#include <sourcepp/Math/Plane.h>
#include <sourcepp/Math/ParametricLine.h>