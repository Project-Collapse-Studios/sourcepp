#pragma once
#include <cinttypes>
#include <sourcepp/math/Vec.h>
#include "VMFBSP.h"

class DisplacementColumns
{
    std::vector<mathpp::Vec3f> normals;
    std::vector<float> distances;
    std::vector<mathpp::Vec3f> offsets;
    std::vector<mathpp::Vec3f> offsetNormals;
    std::vector<float> alpha;
    std::vector<mathpp::Vec2f> triangleTags;
    std::unordered_map<int, int> allowedVerts;

public:
    explicit DisplacementColumns(const KeyValues &kv);
};

class DisplacementInfo
{
    std::uint8_t power;
    sourcepp::math::Vec3f startPosition;
    float elevation;
    bool subdivided;
    int flags;
    sourcepp::math::Vec3f uAxis;
    sourcepp::math::Vec3f vAxis;
    int mintess;
    float smooth;
    DisplacementColumns displacementColumns;

public:
    explicit DisplacementInfo(const KeyValues &kv);

};