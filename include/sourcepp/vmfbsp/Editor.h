#pragma once
#include <sourcepp/Math.h>
#include "kvpp/KV1.h"

class EditorInfo {

public:
    EditorInfo(const kvpp::KV1ElementReadable<> &kv);

    mathpp::Vec3i8 color;
    std::vector<unsigned int> visgroupid;
    unsigned int group;
    bool visgroupshown;
    bool visgroupautoshown;
    std::string_view comments;
    mathpp::Vec2f logicalpos;
};