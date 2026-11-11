#pragma once
#include <cstdint>

class lump_base {

public:
    [[nodiscard]] uint32_t getVersion() const noexcept {
        return this->version;
    }

    void setVersion(const uint32_t version_) noexcept {
        this->version = version_;
    }

protected:
    uint32_t version = 0;

    explicit lump_base(const uint32_t version): version(version) {
    }
};
