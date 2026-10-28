#pragma once

#include <cstddef>
#include <span>


namespace sourcepp::crypto {

uint32_t computeCRC32(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
