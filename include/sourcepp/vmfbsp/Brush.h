#pragma once
#include "BrushFace.h"

const int MAX_BRUSH_FACES = 128;

/// Implements a Brush in BSP/VMF files.
class Brush {
    unsigned int id = 0;
    BrushFace sides[MAX_BRUSH_FACES];

    Brush()
};