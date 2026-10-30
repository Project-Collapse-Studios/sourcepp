#include <sourcepp/vmfbsp/Material.h>
#include <iostream>


Material::Material(const KeyValues& kv) {
    this->mat_file.emplace<1>(kv["material"].getValue());

    using namespace sourcepp::string;
    // UAxis
    auto splits = split(kv["uaxis"].getValue(), ' ');
    if (splits.size() < 5) { // If more, it will get ignored
        throw invalid_uaxis_exception_error();
    }
    toFloatOmitInvalid(splits[0], this->u_axis_dir[0]);
    toFloatOmitInvalid(splits[1], this->u_axis_dir[1]);
    toFloatOmitInvalid(splits[2], this->u_axis_dir[2]);

    toIntOmitInvalid(splits[3], this->u_offset);
    toFloatOmitInvalid(splits[4], this->u_scale);

    // VAxis
    splits = split(kv["vaxis"].getValue(), ' ');
    if (splits.size() < 5) { // If more, it will get ignored
        throw invalid_vaxis_exception_error();
    }
    toFloatOmitInvalid(splits[0], this->v_axis_dir[0]);
    toFloatOmitInvalid(splits[1], this->v_axis_dir[1]);
    toFloatOmitInvalid(splits[2], this->v_axis_dir[2]);

    toIntOmitInvalid(splits[3], this->v_offset);
    toFloatOmitInvalid(splits[4], this->v_scale);

    // Other
    this->rotation = kv["rotation"].getValue<float>();
    this->lightmap_scale = kv["lightmapscale"].getValue<int>();
    this->smoothing_group = kv["smoothing_groups"].getValue<int>();
}