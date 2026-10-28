#pragma once

#include <sourcepp/Math/Plane.h>
#include "Material.h"
#include "VMFBSP.h"

class BrushFace : public mathpp::Plane, Material {
    unsigned int id = 0;

public:
    explicit BrushFace(const KeyValues& kv);
};