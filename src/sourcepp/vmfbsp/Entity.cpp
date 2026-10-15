#include "sourcepp/vmfbsp/Entity.h"

using namespace sourcepp::math;
using Keyvalues = kvpp::KV1Writer<key_base>;

Entity::Entity(const std::string& classname_, const Vec3f& origin_) {
    this->classname = classname_;
    this->origin = origin_;
    this->keyvalues = Keyvalues("");
}

Entity::Entity(kvpp::KV1ElementWritable<key_base> keyvalues_) {

    if (!keyvalues_.getChildCount()) { // If this element has no children
        throw std::invalid_argument("Key values must have at least one child");
    }

    for(const auto& value : keyvalues_) {
        if (value.getChildCount()) {
            throw std::invalid_argument("Key values must be flat! (Have no children)");
        }
    }

    if (!keyvalues_.hasChild("classname")) {
        throw std::invalid_argument("Creating entity by keyvalue requires keyvalues to have at least classname specified!");
    }

    this->classname = keyvalues_["classname"].getValue();
    keyvalues_.removeChild("classname");
    this->keyvalues = Keyvalues(keyvalues_);
}

Entity::~Entity() {
    this->outputs.clear();
}