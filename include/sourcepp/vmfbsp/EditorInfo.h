#pragma once
#include "VMFBSP.h"
#include <sourcepp/math/Vec.h>

namespace vmfbsp {
    class EditorInfo {

    public:
        explicit EditorInfo(const KeyValues& kv) {
            this->color = kv("color").getValue<mathpp::Vec3i8, true>();
            this->visgroupid = kv.getChildrenValues<unsigned int>("visgroupid");
            this->group = kv("group").getValue<unsigned int>();
            this->visgroupshown = kv("visgroupshown").getValue<bool>();
            this->visgroupautoshown = kv("visgroupautoshown").getValue<bool>();
            this->comments = kv("comments").getValue();
            this->logicalpos = kv("logicalpos").getValue<mathpp::Vec2f, true>();
        }

        EditorInfo() = default;

        mathpp::Vec3i8 color;
        std::vector<unsigned int> visgroupid;
        unsigned int group = 0;
        bool visgroupshown = true;
        bool visgroupautoshown = true;
        std::string comments;
        mathpp::Vec2f logicalpos;
    };
}