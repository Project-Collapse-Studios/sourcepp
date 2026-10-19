#pragma once

#include <sourcepp/Math/Plane.h>
#include <sourcepp/Math/ParametricLine.h>

bool Intersect(const Plane& plane1, const Plane& plane2, ParametricLine& out);
bool Intersect(const Plane& plane, const ParametricLine& line, sourcepp::math::Vec3f& out);
bool Intersect(const Plane& plane1, const Plane& plane2, const Plane& plane3, sourcepp::math::Vec3f& out);
