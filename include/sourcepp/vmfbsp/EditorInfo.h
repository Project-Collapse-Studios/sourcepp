#pragma once
#include "VMFBSP.h"
#include <sourcepp/math/Vec.h>

namespace vmfbsp {
    class EditorInfo {

    public:
        explicit EditorInfo(const KeyValues& kv) {
            this->color = kv("color");
            this->visgroupid = kv.getChildrenValues<unsigned int>("visgroupid");
            this->group = kv("group");
            this->visgroupshown = kv("visgroupshown");
            this->visgroupautoshown = kv("visgroupautoshown");
            this->comments = kv("comments");
            this->logicalpos = kv("logicalpos");
        }

        EditorInfo() = default;

        mathpp::Vec3i8 color;
        std::vector<unsigned int> visgroupid;
        unsigned int group = 0;
        bool visgroupshown = true;
        bool visgroupautoshown = true;
        std::string comments;
        mathpp::Vec2i logicalpos;
    };
}