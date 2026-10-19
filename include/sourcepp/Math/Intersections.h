#pragma once

#include <sourcepp/Math/Plane.h>
#include <sourcepp/Math/ParametricLine.h>

bool Intersect(const mathpp::Plane& plane1, const mathpp::Plane& plane2, mathpp::ParametricLine& out);
bool Intersect(const mathpp::Plane& plane, const mathpp::ParametricLine& line, mathpp::Vec3f& out);
bool Intersect(const mathpp::Plane& plane1, const mathpp::Plane& plane2, const mathpp::Plane& plane3, mathpp::Vec3f& out);
