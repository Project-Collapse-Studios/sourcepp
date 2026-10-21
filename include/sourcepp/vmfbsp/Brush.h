#pragma once
#include "BrushFace.h"
#include <kvpp/KV1.h>


constexpr int MAX_BRUSH_FACES = 128;

/// Implements a Brush in BSP/VMF files.
class Brush {
    unsigned int id = 0;
    std::vector<BrushFace> sides;

public:
    explicit Brush(const kvpp::KV1ElementReadable<>& kv);
};