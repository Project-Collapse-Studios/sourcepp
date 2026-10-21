#pragma once

#include <sourcepp/Math/Plane.h>
#include "Material.h"
#include <kvpp/KV1.h>

class BrushFace : public mathpp::Plane, Material {
    unsigned int id = 0;

public:
    explicit BrushFace(const kvpp::KV1ElementReadable<>& kv);
};