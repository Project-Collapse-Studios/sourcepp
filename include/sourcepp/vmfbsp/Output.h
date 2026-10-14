#pragma once

#include <variant>
#include <vector>
#include "Entity.h"

class Entity;

using TargetOrEntityList = std::variant<std::string, std::vector<Entity*>>;
using OutputEntityIterator = std::vector<Entity*>::iterator;


/// Represents an I/O event between entities configured inside VMF/BSP files.
class Output {
public:
    /// Construct an Output.
    Output(const std::string_view& output_name, const TargetOrEntityList& target, const std::string_view& input_name, float delay = 0, int num_fires = -1) :
        output_name(output_name),
        target(target),
        input_name(input_name),
        delay(delay),
        num_fires(num_fires) {}


    /// Iterate over targets. Only useful if target holds a vector of Entities.
    [[nodiscard]] OutputEntityIterator begin() {
        if (auto* val = std::get_if<std::vector<Entity*>>(&this->target)) { // We're holding a vector
            return val->begin();
        }
        return empty_vec.begin();
    }

    [[nodiscard]] OutputEntityIterator end() {
        if (auto* val = std::get_if<std::vector<Entity*>>(&this->target)) { // We're holding a vector
            return val->end();
        }
        return empty_vec.end();
    }

    // These are public!
    std::string_view output_name;
    TargetOrEntityList target;
    std::string_view input_name;
    float delay = 0;
    int num_fires = 0;

private:
    inline static std::vector<Entity*> empty_vec;

};