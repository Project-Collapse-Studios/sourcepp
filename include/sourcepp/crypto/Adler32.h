#pragma once

#include <cstddef>
#include <span>
namespace sourcepp::crypto {

uint32_t computeAdler32(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
