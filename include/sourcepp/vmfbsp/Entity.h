#pragma once

#include <string>
#include <sourcepp/Math.h>
#include <kvpp/KV1Writer.h>
#include "Output.h"

#ifdef Use_STRINGVIEW
using key_base = std::string_view;
#else
using key_base = std::string;
#endif

/*
 * This class represents an Point Entity in the source engine.
 * It stores the classname, origin and additional keyvalues.
 * It's intended to be used in VMFs or BSPs, thus you cannot create this class directly. You can only create an std::unique_ptr to it, as it is enforced to only have one owner at a time.
 * In case you want to modify it: Remove it from the map, perform your modifications and add it back in.
 *
 * In order to create an Entity, call Entity::Create()
 * There are a couple of options:
 * 1. Create from classname, origin
 * 2. Create from Keyvalues. This class supports creating from KV1, KV1Writer, KV1ElementReadable, and KV1Writable. Internally every class of keyvalues gets converted to KV1ElementWritable. When passing KV1 or KV1Writer, the root is omitted.
 */
class Entity {

public:
        // Constructors and destructors
        Entity() = delete;

        [[nodiscard]] static std::unique_ptr<Entity> Create(const std::string& classname_, const sourcepp::math::Vec3f& origin_ = sourcepp::math::Vec3f(0.0f, 0.0f, 0.0f)) {
                return std::unique_ptr<Entity>(new Entity(classname_, origin_));
        }

        [[nodiscard]] static std::unique_ptr<Entity> Create(const kvpp::KV1Writer<key_base>& kv) {
            return std::unique_ptr<Entity>(new Entity(kv[0])); // In case of base classes that contain the root, omit the root
        }

        [[nodiscard]] static std::unique_ptr<Entity> Create(const kvpp::KV1<key_base>& kv) {
            return std::unique_ptr<Entity>(new Entity(kvpp::KV1Writer(kv)[0])); // The entity cannot be read only.
        }

        [[nodiscard]] static std::unique_ptr<Entity> Create(const kvpp::KV1ElementWritable<key_base>& kv) {
            return std::unique_ptr<Entity>(new Entity(kv));
        }

        [[nodiscard]] static std::unique_ptr<Entity> Create(const kvpp::KV1ElementReadable<key_base>& kv) {
            return std::unique_ptr<Entity>(new Entity(kvpp::KV1ElementWritable<key_base>(kv)));
        }

        [[nodiscard]] static std::unique_ptr<Entity> Create(const std::string_view& keyvalues) {
            return std::unique_ptr<Entity>(new Entity(kvpp::KV1Writer<key_base>(keyvalues)[0]));
        }

        ~Entity();
        // Getters

        [[nodiscard]] std::string GetClassname() const {
            return this->classname;
        }

        void SetClassname(const std::string& name) {
            this->classname = name;
        }

        [[nodiscard]] sourcepp::math::Vec3f GetOrigin() const {
            return this->origin;
        }

        void SetOrigin(const sourcepp::math::Vec3f& origin_) {
            this->origin = origin_;
        }

        // Returns keyvalues. Read-only
        [[nodiscard]] kvpp::KV1<key_base> GetAllKeyValues() const {
            return kvpp::KV1{this->keyvalues};
        }

        void SetKeyValue(const key_base& key, const std::string& value) {
            this->keyvalues[key] = value;
        }

        void RemoveKeyValue(const key_base& key) {
            this->keyvalues.removeChild(key);
        }

        /// Retrieve the value of a specific key. Use the template syntax to get the appropriate data type.
        template <kvpp::KV1ValueType S>
        [[nodiscard]] S GetKeyValue(const key_base& key) const {
            return this->keyvalues[key].getValue<S>();
        }

        /// Add an output to this entity. This entity claims ownership of this output!
        void AddOutput(const Output pOut) { // We want a copy, to claim ownership of the output.
            this->outputs.push_back(pOut);
        }

        /// Removes and returns the output of out_idx from the output list.
        Output RemoveOutput(const unsigned int out_idx) {
            return *this->outputs.erase(this->outputs.begin() + out_idx);
        }


        /// Allow for entity copying.
        std::unique_ptr<Entity> copy() const {
            return std::unique_ptr<Entity>(new Entity(*this)); // Std::make_unique requires access to the protected copy constructor
            }

    protected:
        kvpp::KV1Writer<key_base> keyvalues;
        std::string classname;
        sourcepp::math::Vec3f origin;
        std::vector<Output> outputs;

        // Constructors:
        explicit Entity(const std::string&, const sourcepp::math::Vec3f& origin_ = sourcepp::math::Vec3f(0.0f, 0.0f, 0.0f));
        explicit Entity(kvpp::KV1ElementWritable<key_base> keyvalues_);
        // TODO:
        // Entity(kv_binary);
        explicit Entity(const Entity& other) {
            // Copy constructor. We disallow traditional copies from outside.
            this->keyvalues = other.keyvalues;
            this->classname = other.classname;
            this->origin = other.origin;
            for (auto pOutput = other.outputs.begin(); pOutput != other.outputs.end(); pOutput++) {
                this->outputs.emplace_back(*pOutput); // Copy the output
            }
        }
};