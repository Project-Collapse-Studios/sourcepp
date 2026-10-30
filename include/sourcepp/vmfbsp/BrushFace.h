#pragma once

#include <sourcepp/math/Plane.h>
#include "Material.h"
#include "VMFBSP.h"
#include "DisplacementInfo.h"

class BrushFace : public mathpp::Plane, Material {
    struct too_few_points_exception : public std::exception
    {
        const char * what() const noexcept override
        {
            return "Cannot create BrushFace with fewer than 3 points!";
        }
    };

    unsigned int id = 0;
    DisplacementInfo displacementInfo;

public:
    explicit BrushFace(const KeyValues& kv);
};