#pragma once
#include "BrushFace.h"
#include <kvpp/KV1.h>


constexpr int MAX_BRUSH_FACES = 128;

/// Implements a Brush in BSP/VMF files.
class Brush {
    unsigned int id = 0;
    BrushFace sides[MAX_BRUSH_FACES];

    explicit Brush(const kvpp::KV1ElementReadable<>& kv);
};