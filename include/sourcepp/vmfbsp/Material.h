#pragma once
#include <variant>
#include <kvpp/specialized/VMT.h>
#include <sourcepp/math/Vec.h>
#include "VMFBSP.h"

using VMT_or_String = std::variant<const VMT*, std::string>;

class Material {

    struct invalid_uaxis_exception_error : public std::exception
    {
        const char * what() const noexcept override
        {
            return "Invalid uaxis value!";
        };
    };

    struct invalid_vaxis_exception_error : public std::exception
    {
        const char * what() const noexcept override
        {
            return "Invalid vaxis value!";
        };
    };

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
    Material() = default;
    explicit Material(const KeyValues& kv);
};