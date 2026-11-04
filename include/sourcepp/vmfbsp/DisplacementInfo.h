#pragma once
#include <cinttypes>
#include "VMFBSP.h"


enum TriangleTags {
    SMALL_SLOPE = 9, // Insignificant slope, player can walk on it
    MEDIUM_SLOPE = 1, // Significant slope, but player can still walk on it
    STEEP_SLOPE = 0 // Large slope, player cannot walk on it
};

class DisplacementInfo {

    public:
        uint8_t power = 0;
        mathpp::Vec3f start_position;
        float elevation = 0;
        bool subdiv = false;
        uint8_t flags = 0;

        /// NOTICE:
        /// First coordinate is Y, second is X
        std::vector<  std::vector< mathpp::Vec3f >  > normals;
        std::vector<  std::vector< float >  > distances;
        std::vector<  std::vector< mathpp::Vec3f >  > offsets;
        std::vector<  std::vector< mathpp::Vec3f >  > offset_normals;
        std::vector<  std::vector< float >  > alphas;
        std::vector<  std::array<TriangleTags, 2>  > triangle_tags;

        std::vector< size_t > allowed_verts;

        explicit DisplacementInfo(const KeyValues& kv);
        DisplacementInfo() = default;
};