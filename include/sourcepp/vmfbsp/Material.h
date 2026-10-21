#pragma once
#include <variant>
#include <kvpp/specialized/VMT.h>
#include <kvpp/KV1.h>
#include <sourcepp/Math.h>

using VMT_or_String = std::variant<const VMT*, std::string_view>;

class Material {
    VMT_or_String mat_file;

    // UV Axes
    mathpp::Vec3f u_axis_dir;
    int u_offset = 0;
    float u_scale = 0;

    mathpp::Vec3f v_axis_dir;
    int v_offset = 0;
    float v_scale = 0;

    float rotation = 0;
    unsigned int lightmap_scale = 16;
    unsigned short smoothing_group = 0;

public:
    explicit Material(const kvpp::KV1ElementReadable<>& kv);
};