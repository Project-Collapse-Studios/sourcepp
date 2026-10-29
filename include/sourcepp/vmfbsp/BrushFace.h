#pragma once

#include <sourcepp/math/Plane.h>
#include "Material.h"
#include "VMFBSP.h"

class BrushFace : public mathpp::Plane, Material {
    unsigned int id = 0;

public:
    explicit BrushFace(const KeyValues& kv);
};