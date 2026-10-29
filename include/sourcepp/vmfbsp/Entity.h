#pragma once

#include <string>
#include <sourcepp/math/Vec.h>
#include <kvpp/KV1.h>
#include "Output.h"
#include "VMFBSP.h"
/*
 * This class represents an Point Entity in the source engine.
 * It stores the classname, origin and additional keyvalues.
 * It's intended to be used in VMFs or BSPs, thus you cannot create this class directly. You can only create an std::unique_ptr to it, as it is enforced to only have one owner at a time.
 * In case you want to modify it: Remove it from the map, perform your modifications and add it back in.
 *
 * In order to create an Entity, call Entity::Create()
 * There are a couple of options:
 * 1. Create from classname, origin
 * 2. Create from Keyvalues. This class supports creating from KV1 and KV1Element.
 */
namespace vmfbsp {
    class Entity {

        using KeyValues = kvpp::KV1Element<std::string, std::string>;
        using KeyValuesRoot = kvpp::KV1<std::string, std::string>;

    public:
        // Constructors and destructors
        Entity() = delete;

        [[nodiscard]] static std::unique_ptr<Entity> CreateByComponents(const std::string& classname_, const mathpp::Vec3f& origin_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f), const mathpp::EulerAngles& angles_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f)) {
            return std::unique_ptr<Entity>(new Entity(classname_, origin_, angles_));
        }

        [[nodiscard]] static std::unique_ptr<Entity> CreateByKeyvalues(const KeyValues& kv) {
            return std::unique_ptr<Entity>(new Entity(KeyValuesRoot{kv}));
        }

        ~Entity();
        // Getters

        [[nodiscard]] std::string GetClassname() const {
            return this->classname;
        }

        void SetClassname(const std::string& name) {
            this->classname = name;
        }

        [[nodiscard]] mathpp::Vec3f GetOrigin() const {
            return this->origin;
        }

        void SetOrigin(const mathpp::Vec3f& origin_) {
            this->origin = origin_;
        }

        // Returns keyvalues. Read-only
        [[nodiscard]] const KeyValuesRoot& GetAllKeyValues() const {
            return this->keyvalues;
        }

        void SetKeyValue(const std::string& key, const std::string& value) {
            this->keyvalues[key].setValue(value);
        }

        void RemoveKeyValue(const std::string& key) {
            this->keyvalues.removeChild(key);
        }

        /// Retrieve the value of a specific key. Use the template syntax to get the appropriate data type.
        template <kvpp::KV1ValueType S>
        [[nodiscard]] S GetKeyValue(const std::string& key) const {
            return this->keyvalues[key].getValue<S>();
        }

        /// Return if the a specific spawnflag (or a combination of flags) is set
        [[nodiscard]] bool GetSpawnflag(const unsigned int flag) const {
            return (this->spawnflags & flag) == flag;
        }

        /// Set a spawnflag (or a combinations of such)
        void SetSpawnflag(const unsigned int flags) {
            this->spawnflags |= flags;
        }

        /// Overrides the spawnflags completely and sets them to the passed value.
        void OverrideSpawnflag(const unsigned int spawnflags_) {
            this->spawnflags = spawnflags_;
        }

        /// Unset a spawnflag (or a combinations of such)
        void UnSetSpawnflag(const unsigned int flags) {
            this->spawnflags ^= this->spawnflags & flags; // This will give us a "mask" so that when a flag is already unset it doesn't get set.
        }

        /// Add an output to this entity.
        void AddOutput(const Output& pOut) { // We want a copy, to claim ownership of the output.
            this->outputs.push_back(pOut);
        }

        /// Removes and returns the output of out_idx from the output list.
        Output RemoveOutput(const unsigned int out_idx) {
            return *this->outputs.erase(this->outputs.begin() + out_idx);
        }


        /// Copy the entity. Returns a new object based on this one.
        [[nodiscard]] std::unique_ptr<Entity> copy() const {
            return std::unique_ptr<Entity>(new Entity(*this)); // Std::make_unique requires access to the protected copy constructor
        }

    protected:
        KeyValuesRoot keyvalues;
        std::string classname;
        mathpp::Point origin;
        mathpp::EulerAngles angles;
        std::vector<Output> outputs;
        std::vector<Brush> brushes;
        unsigned int spawnflags;

        // Constructors:
        explicit Entity(const std::string&, const mathpp::Vec3f& origin_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f), const mathpp::EulerAngles& angles_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f));
        explicit Entity(const KeyValuesRoot& keyvalues_);
        // TODO:
        // Entity(kv_binary);
        explicit Entity(const Entity& other) :
            keyvalues(other.keyvalues),
            classname(other.classname),
            origin(other.origin),
            spawnflags(other.spawnflags)
        {
            // Copy constructor. We disallow traditional copies from outside.
            for (auto pOutput = other.outputs.begin(); pOutput != other.outputs.end(); ++pOutput) {
                this->outputs.emplace_back(*pOutput); // Copy the output
            }
        }
    };
}