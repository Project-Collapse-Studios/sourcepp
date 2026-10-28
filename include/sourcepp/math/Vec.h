#pragma once

#include "Math.h"

namespace mathpp {
	template<uint8_t S_, Arithmetic P>
	struct Vec {
		static constexpr uint8_t S = S_;
		static_assert(S >= 2, "Vectors must have at least two values!");

		using value_type = P;

		P values[S];

		constexpr Vec() = default;

		// ReSharper disable once CppNonExplicitConvertingConstructor
		template<std::convertible_to<P>... Vals>
		requires (sizeof...(Vals) == S)
		constexpr Vec(Vals... vals) // NOLINT(*-explicit-constructor)
				: values{static_cast<P>(vals)...} {}

		[[nodiscard]] constexpr const P* data() const {
			return this->values;
		}

		[[nodiscard]] constexpr P* data() {
			return this->values;
		}

		[[nodiscard]] constexpr uint8_t size() const {
			return S;
		}

		[[nodiscard]] constexpr P& operator[](uint8_t index) {
			if (index < S) {
				return this->values[index];
			}
			return this->operator[](index % S);
		}

		[[nodiscard]] constexpr P operator[](uint8_t index) const {
			if (index < S) {
				return this->values[index];
			}
			return this->operator[](index % S);
		}

		[[nodiscard]] constexpr Vec operator+() const {
			return *this;
		}

		[[nodiscard]] constexpr Vec operator+(Arithmetic auto scalar) const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] += static_cast<P>(scalar);
			}
			return out;
		}

		template<uint8_t SO, Arithmetic PO>
		[[nodiscard]] constexpr Vec operator+(const Vec<SO, PO>& other) const {
			auto out = *this;
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				out[i] += static_cast<P>(other[i]);
			}
			return out;
		}

		constexpr void operator+=(Arithmetic auto scalar) const {
			for (uint8_t i = 0; i < S; i++) {
				(*this)[i] += static_cast<P>(scalar);
			}
		}

		template<uint8_t SO, Arithmetic PO>
		constexpr void operator+=(const Vec<SO, PO>& other) {
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				(*this)[i] += static_cast<P>(other[i]);
			}
		}

		[[nodiscard]] constexpr Vec operator-() const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] *= -1;
			}
			return out;
		}

		[[nodiscard]] constexpr Vec operator-(Arithmetic auto scalar) const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] -= static_cast<P>(scalar);
			}
			return out;
		}

		template<uint8_t SO, Arithmetic PO>
		[[nodiscard]] constexpr Vec operator-(const Vec<SO, PO>& other) const {
			auto out = *this;
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				out[i] -= static_cast<P>(other[i]);
			}
			return out;
		}

		constexpr void operator-=(Arithmetic auto scalar) const {
			for (uint8_t i = 0; i < S; i++) {
				(*this)[i] -= static_cast<P>(scalar);
			}
		}

		template<uint8_t SO, Arithmetic PO>
		constexpr void operator-=(const Vec<SO, PO>& other) {
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				(*this)[i] -= static_cast<P>(other[i]);
			}
		}

		[[nodiscard]] constexpr Vec operator*(Arithmetic auto scalar) const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] *= static_cast<P>(scalar);
			}
			return out;
		}

		constexpr void operator*=(Arithmetic auto scalar) {
			for (uint8_t i = 0; i < S; i++) {
				(*this)[i] *= static_cast<P>(scalar);
			}
		}

		[[nodiscard]] constexpr Vec operator/(Arithmetic auto scalar) const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] /= static_cast<P>(scalar);
			}
			return out;
		}

		constexpr void operator/=(Arithmetic auto scalar) {
			for (uint8_t i = 0; i < S; i++) {
				(*this)[i] /= static_cast<P>(scalar);
			}
		}

		[[nodiscard]] constexpr Vec operator%(Arithmetic auto scalar) const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] %= static_cast<P>(scalar);
			}
			return out;
		}

		constexpr void operator%=(Arithmetic auto scalar) {
			for (uint8_t i = 0; i < S; i++) {
				(*this)[i] %= static_cast<P>(scalar);
			}
		}

		template<uint8_t SO, Arithmetic PO>
		[[nodiscard]] constexpr bool operator==(const Vec<SO, PO>& other) const {
			if constexpr (S != SO) {
				return false;
			} else {
				for (uint8_t i = 0; i < S; i++) {
					if ((*this)[i] != static_cast<P>(other[i])) {
						return false;
					}
				}
				return true;
			}
		}

		template<uint8_t SO, Arithmetic PO = P>
		[[nodiscard]] constexpr Vec<SO, PO> to() const {
			Vec<SO, PO> out{};
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				out[i] = static_cast<PO>((*this)[i]);
			}
			return out;
		}

		template<uint8_t SO, Arithmetic PO>
		[[nodiscard]] constexpr Vec mul(const Vec<SO, PO>& other) const {
			auto out = *this;
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				out[i] *= static_cast<P>(other[i]);
			}
			return out;
		}

		template<uint8_t SO, Arithmetic PO>
		[[nodiscard]] constexpr Vec div(const Vec<SO, PO>& other) const {
			auto out = *this;
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				out[i] /= static_cast<P>(other[i]);
			}
			return out;
		}

		template<uint8_t SO, Arithmetic PO>
		[[nodiscard]] constexpr Vec mod(const Vec<SO, PO>& other) const {
			auto out = *this;
			for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
				if constexpr ((std::floating_point<P> && std::floating_point<PO>) || std::floating_point<P>) {
					out[i] = std::fmod(out[i], static_cast<P>(other[i]));
				} else {
					out[i] %= static_cast<P>(other[i]);
				}
			}
			return out;
		}

		[[nodiscard]] constexpr float magf() const {
			float out = 0.0;
			for (uint8_t i = 0; i < S; i++) {
				out += std::pow((*this)[i], 2);
			}
			return std::sqrt(out);
		}

		[[nodiscard]] constexpr double mag() const {
			double out = 0.0;
			for (uint8_t i = 0; i < S; i++) {
				out += std::pow((*this)[i], 2);
			}
			return std::sqrt(out);
		}

		[[nodiscard]] constexpr P sum() const {
			P out{};
			for (uint8_t i = 0; i < S; i++) {
				out += (*this)[i];
			}
			return out;
		}

		template<Arithmetic PO>
		[[nodiscard]] constexpr Vec scale(const Vec<S, PO>& other) const {
			Vec out;
			for (uint8_t i = 0; i < S; i++) {
				out[i] = (*this)[i] * static_cast<P>(other[i]);
			}
			return out;
		}

		template<Arithmetic PO>
		[[nodiscard]] constexpr P dot(const Vec<S, PO>& other) const {
			return this->scale(other).sum();
		}

		template<Arithmetic PO>
		[[nodiscard]] constexpr Vec<3, PO> cross(const Vec<3, PO>& other) const requires (S_ == 3) {
			// Cross product is only defined for 3 and 7 dimensional vectors.
			// I WILL NOT program in a 7 dimensional cross product.
			return Vec<3, PO>(
				this->operator[](1) * other[2] - this->operator[](2) * other[1],
				this->operator[](2) * other[0] - this->operator[](0) * other[2],
				this->operator[](0) * other[1] - this->operator[](1) * other[0]
				);
		}

		constexpr float norm() {
			float len = std::sqrt(this->dot(*this));
			this->operator=(*this / len);
			return len;
		}

		[[nodiscard]] constexpr Vec abs() const {
			auto out = *this;
			for (uint8_t i = 0; i < S; i++) {
				out[i] = std::abs(out[i]);
			}
			return out;
		}

		[[nodiscard]] static constexpr Vec zero() {
			return {};
		}

		[[nodiscard]] constexpr bool isZero() const {
			return *this == zero();
		}

		friend std::ostream& operator<<(std::ostream& stream, const Vec<S_, P>& in) {
			stream << "[";
			for (uint8_t i = 0; i < S_ - 1; ++i) {
				stream << in.values[i] << ", ";
			}
			stream << in.values[S_ - 1] << "]";
			return stream;
		}
	};
	static_assert(std::is_trivially_copyable_v<Vec<2, float>>);

#define SOURCEPP_VEC_DEFINE(S) \
	template<Arithmetic P> \
	using Vec##S       = Vec<S, P>; \
	using Vec##S##i8   = Vec##S<int8_t>; \
	using Vec##S##i16  = Vec##S<int16_t>; \
	using Vec##S##i32  = Vec##S<int32_t>; \
	using Vec##S##i64  = Vec##S<int64_t>; \
	using Vec##S##i    = Vec##S##i32; \
	using Vec##S##ui8  = Vec##S<uint8_t>; \
	using Vec##S##ui16 = Vec##S<uint16_t>; \
	using Vec##S##ui32 = Vec##S<uint32_t>; \
	using Vec##S##ui64 = Vec##S<uint64_t>; \
	using Vec##S##ui   = Vec##S##ui32; \
	using Vec##S##f16  = Vec##S<half>; \
	using Vec##S##f32  = Vec##S<float>; \
	using Vec##S##f64  = Vec##S<double>; \
	using Vec##S##f    = Vec##S##f32; \
	using Vec##S##d    = Vec##S##f64

	SOURCEPP_VEC_DEFINE(2);
	SOURCEPP_VEC_DEFINE(3);
	SOURCEPP_VEC_DEFINE(4);

#undef SOURCEPP_VEC_DEFINE

	using EulerAngles = Vec3f;
	using Quat = Vec4f;
	using Point = Vec3f;

	/// Lower precision Quat compressed to 6 bytes
struct QuatCompressed48 {
	uint16_t x : 16;
	uint16_t y : 16;
	uint16_t z : 15;
	uint16_t wn : 1;

	[[nodiscard]] Quat decompress() const {
		// Convert from 16-bit (or 15-bit) integers to floating point values in the range [-1, 1]
		const float fx = (static_cast<float>(this->x) / 32767.5f) - 1.f; // x / ((2^16 - 1) / 2) - 1
		const float fy = (static_cast<float>(this->y) / 32767.5f) - 1.f; // y / ((2^16 - 1) / 2) - 1
		const float fz = (static_cast<float>(this->z) / 16383.5f) - 1.f; // z / ((2^15 - 1) / 2) - 1

		// Recalculate w from the constraint that x^2 + y^2 + z^2 + w^2 = 1
		float fw = std::sqrt(1.f - fx * fx - fy * fy - fz * fz);

		// Adjust w based on the stored sign bit
		if (this->wn) {
			fw = -fw;
		}

		return {fx, fy, fz, fw};
	}
};
static_assert(std::is_trivially_copyable_v<QuatCompressed48>);

/// Lower precision Quat compressed to 8 bytes
struct QuatCompressed64 {
	uint32_t x : 21;
	uint32_t y : 21;
	uint32_t z : 21;
	uint32_t wn : 1;

	[[nodiscard]] Quat decompress() const {
		// Convert from 21-bit integers to floating point values in the range [-1, 1]
		const double fx = (static_cast<double>(this->x) / 1048575.5) - 1.0f; // x / ((2^21 - 1) / 2) - 1
		const double fy = (static_cast<double>(this->y) / 1048575.5) - 1.0f; // y / ((2^21 - 1) / 2) - 1
		const double fz = (static_cast<double>(this->z) / 1048575.5) - 1.0f; // z / ((2^21 - 1) / 2) - 1

		// Recalculate w from the constraint that x^2 + y^2 + z^2 + w^2 = 1
		double fw = std::sqrt(1.0 - fx * fx - fy * fy - fz * fz);

		// Adjust w based on the stored sign bit
		if (this->wn) {
			fw = -fw;
		}

		return {static_cast<float>(fx), static_cast<float>(fy), static_cast<float>(fz), static_cast<float>(fw)};
	}
};
static_assert(std::is_trivially_copyable_v<QuatCompressed64>);

/// Lower precision Vec3 compressed to 6 bytes
struct Vec3Compressed48 : Vec3ui16 {
	using Vec3ui16::Vec;

	[[nodiscard]] Vec3f decompress() const {
		const float fx = (static_cast<float>((*this)[0]) / 32767.5f) - 1.f;
		const float fy = (static_cast<float>((*this)[1]) / 32767.5f) - 1.f;
		const float fz = (static_cast<float>((*this)[2]) / 32767.5f) - 1.f;
		return {fx, fy, fz};
	}
};
static_assert(std::is_trivially_copyable_v<Vec3Compressed48>);

}