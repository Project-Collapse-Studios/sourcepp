#pragma once

#include <array>
#include <cstddef>
#include <span>


namespace sourcepp::crypto {

std::array<std::byte, 20> computeSHA1(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
