#pragma once
#include <kvpp/KV1.h>
#include <sourcepp/vmfbsp/Entity.h>
#include "lump_base.h"

class EntityLump : public lump_base {

public:
    using KeyValues = vmfbsp::Entity::KeyValues;


    explicit EntityLump(std::span<std::byte> raw_data, uint32_t version);
    explicit EntityLump(std::string_view raw_str_data, uint32_t version);


protected:
    std::vector< vmfbsp::Entity > Entities;
};
