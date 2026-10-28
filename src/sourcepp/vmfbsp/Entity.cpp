#include "sourcepp/vmfbsp/Entity.h"
#ifdef DUPA
using namespace mathpp;
using Keyvalues = kvpp::KV1Writer<key_base>;

Entity::Entity(const std::string& classname_, const Vec3f& origin_, const EulerAngles& angles_) {
    this->classname = classname_;
    this->origin = origin_;
    this->angles = angles_;
    this->keyvalues = Keyvalues("");
    this->spawnflags = 0;
}

Entity::Entity(const kvpp::KV1ElementWritable<key_base>& keyvalues_) :
    keyvalues(keyvalues_) { // Cast KV1ElementWritable onto KV1Writer

    // Check if our values don't have children of chlidren
    for (const auto &value: this->keyvalues) {
        if (value.getKey() == "connections") {continue;}
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
        for (auto& connections = this->keyvalues["connections"]; auto& connection : connections) {
            this->outputs.emplace_back(connection);
        }
        this->keyvalues.removeChild("connections");
    }
}

Entity::~Entity() {
    this->outputs.clear();
}
#endif