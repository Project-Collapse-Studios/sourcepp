#pragma once

#include <string>
#include <sourcepp/Math.h>
#include <kvpp/KV1Writer.h>

#ifdef Use_STRINGVIEW
using key_base = std::string;
#else
using key_base = std::string;
#endif

class Entity {
    public:
        // Constructors and destructors
        explicit Entity(const std::string&, const sourcepp::math::Vec3f& origin_ = sourcepp::math::Vec3f(0.0f, 0.0f, 0.0f));
        explicit Entity(const kvpp::KV1Writer<key_base>&);

        Entity(kvpp::KV1ElementWritable<key_base> keyvalues_);
        // TODO:
        // Entity(kv_binary);
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

    protected:
        kvpp::KV1Writer<key_base> keyvalues;
        std::string classname;
        sourcepp::math::Vec3f origin;
        // TODO: Output vector
};