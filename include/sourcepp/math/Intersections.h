#pragma once

#include "Plane.h"
#include "ParametricLine.h"
#include "Vec.h"

bool Intersect(const mathpp::Plane& plane1, const mathpp::Plane& plane2, mathpp::ParametricLine& out);
bool Intersect(const mathpp::Plane& plane, const mathpp::ParametricLine& line, mathpp::Point& out);
bool Intersect(const mathpp::Plane& plane1, const mathpp::Plane& plane2, const mathpp::Plane& plane3, mathpp::Point& out);
