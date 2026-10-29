#include "sourcepp/vmfbsp/Entity.h"

using namespace mathpp;
using namespace vmfbsp;

Entity::Entity(const std::string& classname_, const Vec3f& origin_, const EulerAngles& angles_) {
    this->classname = classname_;
    this->origin = origin_;
    this->angles = angles_;
    this->keyvalues = KeyValuesRoot{""};
    this->spawnflags = 0;
}

Entity::Entity(const KeyValuesRoot& keyvalues_) :
    keyvalues(keyvalues_) {

    // Check if our values don't have children of chlidren
    for (const auto &value: this->keyvalues) {
        if (value.getKey() == "connections" || value.getKey() == "editor") {continue;}
        if (value.getChildCount()) {
            throw std::invalid_argument("Key values must be flat! (Have no children)");
        }
    }

    if (!this->keyvalues.hasChild("classname")) {
        throw std::invalid_argument("Cannot locate classname definition in the keyvalue structure!");
    }

    this->classname = this->keyvalues["classname"].getValue();
    this->keyvalues.removeChild("classname");

    if (this->keyvalues.hasChild("spawnflags")) {
        this->spawnflags = this->keyvalues["spawnflags"].getValue<unsigned int>();
        this->keyvalues.removeChild("spawnflags");
    } else {
        this->spawnflags = 0;
    }

    if (this->keyvalues.hasChild("angles")) {
        this->angles = this->keyvalues["angles"].getValue<EulerAngles>();
        this->keyvalues.removeChild("angles");
    } else {
        this->angles = EulerAngles(0.f, 0.f, 0.f);
    }

    if (this->keyvalues.hasChild("origin")) {
        this->origin = this->keyvalues["origin"].getValue<Vec3f>();
        this->keyvalues.removeChild("origin");
    } else {
        this->origin = Vec3f(0.f, 0.f, 0.f);
    }

    if (this->keyvalues.hasChild("connections")) {
        auto& connections = this->keyvalues["connections"];
        for (auto& connection : connections) {
            this->outputs.emplace_back(connection);
        }
        this->keyvalues.removeChild("connections");
    }
}

Entity::~Entity() {
    this->outputs.clear();
}