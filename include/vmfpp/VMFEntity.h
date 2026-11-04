#pragma once

#include <sourcepp/vmfbsp/Entity.h>
#include <vmfpp/EditorInfo.h>

/// VMF representation of an entity, including properties only found in the VMF
class VMFEntity : public vmfbsp::Entity {


protected:
    vmfpp::EditorInfo editor_info{};

    explicit VMFEntity(const KeyValuesRoot& keyvalues_) : Entity(keyvalues_) {
        if (keyvalues_.hasChild("editor")) {
            this->editor_info = vmfpp::EditorInfo(keyvalues_("editor"));
        }
    }

    explicit VMFEntity(const std::string& str, const mathpp::Vec3f& origin_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f), const mathpp::EulerAngles& angles_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f)) :
        Entity(str, origin_, angles_) {

    }


public:
    ~VMFEntity() override = default;

    [[nodiscard]] static std::unique_ptr<VMFEntity> CreateByComponents(const std::string& classname_, const mathpp::Vec3f& origin_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f), const mathpp::EulerAngles& angles_ = mathpp::Vec3f(0.0f, 0.0f, 0.0f)) {
        return std::unique_ptr<VMFEntity>(new VMFEntity(classname_, origin_, angles_));
    }

    [[nodiscard]] static std::unique_ptr<VMFEntity> CreateByKeyvalues(const KeyValues& kv) {
        return std::unique_ptr<VMFEntity>(new VMFEntity(KeyValuesRoot{kv}));
    }

    KeyValuesRoot SerializeToKeyvalues() const override;
};