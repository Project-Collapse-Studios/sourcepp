#include <bsppp/lumps/Entity.h>
#include <iostream>
EntityLump::EntityLump(std::span<std::byte> raw_data, uint32_t version) : lump_base(version) {
    const std::string_view data{reinterpret_cast<char*>(raw_data.data()), raw_data.size()};


    vmfbsp::Entity::KeyValuesRoot kvs{data};

    for (size_t i = 0; i < kvs.getChildCount(); ++i) {
        this->Entities.emplace_back(kvs[i]);
    }
}

EntityLump::EntityLump(std::string_view raw_string_data, uint32_t version) : lump_base(version) {

    std::cout << raw_string_data;
    vmfbsp::Entity::KeyValuesRoot kvs{raw_string_data, true};
    for (size_t i = 0; i < kvs.getChildCount(); ++i) {
        this->Entities.push_back(vmfbsp::Entity{vmfbsp::Entity::KeyValuesRoot{kvs[i]}});
    }
}