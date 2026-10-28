// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppRedundantParentheses

#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <numbers>
#include <type_traits>

#include <half.hpp>

// Numeric types are intentionally outside the sourcepp namespace
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using half_float::half;

namespace mathpp {

template<std::floating_point F>
constexpr F pi = std::numbers::pi_v<F>;
constexpr auto pi_f32  = pi<float>;
constexpr auto pi_f64  = pi<double>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>  || std::is_unsigned_v<T> || std::same_as<T, half>;

template<Arithmetic T>
[[nodiscard]] constexpr T remap(T value, T l1, T h1, T l2, T h2) {
	return l2 + (value - l1) * (h2 - l2) / (h1 - l1);
}

template<Arithmetic T>
[[nodiscard]] constexpr T remap(T value, T h1, T h2) {
	return value * h2 / h1;
}

[[nodiscard]] constexpr bool isPowerOf2(std::unsigned_integral auto n) {
	return n && !(n & (n - 1));
}

template<std::unsigned_integral T>
[[nodiscard]] constexpr T nearestPowerOf2(T n) {
	if (isPowerOf2(n)) {
		return n;
	}
	auto bigger = std::bit_ceil(n);
	auto smaller = std::bit_floor(n);
	return (n - smaller) < (bigger - n) ? smaller : bigger;
}

[[nodiscard]] constexpr uint32_t log2ceil(uint32_t value) {
	return ((std::bit_cast<uint32_t>(static_cast<float>(value)) >> 23) & 0xff) - 127;
}

[[nodiscard]] constexpr uint16_t paddingForAlignment(uint16_t alignment, uint64_t n) {
	if (const auto rest = n % alignment; rest > 0) {
		return alignment - rest;
	}
	return 0;
}

} // namespace mathpp

namespace sourcepp::math
{
    using namespace mathpp;
} // Backwards compat with older sourcepp libs/apps.
