#pragma once

#include <cstddef>
#include <span>


namespace sourcepp::crypto {

std::array<std::byte, 32> computeSHA256(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
