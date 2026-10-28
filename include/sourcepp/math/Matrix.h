#pragma once

#include "Math.h"

namespace mathpp {
template<uint8_t M_, uint8_t N_, Arithmetic P>
class Matrix {
public:
	static constexpr uint8_t M = M_;
	static_assert(M >= 2, "Matrices must have at least two rows!");
	static constexpr uint8_t N = N_;
	static_assert(N >= 2, "Matrices must have at least two columns!");

	using value_type = P;

	P values[M][N];

	[[nodiscard]] constexpr const P* data() const {
		return this->values;
	}

	[[nodiscard]] constexpr P* data() {
		return this->values;
	}

	[[nodiscard]] constexpr uint8_t rows() const {
		return M;
	}

	[[nodiscard]] constexpr uint8_t cols() const {
		return N;
	}

	[[nodiscard]] P* operator[](uint8_t i) { return this->values[i]; }

	[[nodiscard]] const P* operator[](uint8_t i) const { return this->values[i]; }

	[[nodiscard]] P& operator()(uint8_t i, uint8_t j) {
		return this->values[i][j];
	}

	[[nodiscard]] const P& operator()(uint8_t i, uint8_t j) const {
		return this->values[i][j];
	}

	[[nodiscard]] P det() const requires (M == N)
	{
		if constexpr (M == 1) {
			return this->values[0][0];
		} else if constexpr (M == 2) {
			return this->values[0][0] * this->values[1][1] -
				   this->values[0][1] * this->values[1][0];
		} else if constexpr (M == 3) {
			return this->values[0][0] * (this->values[1][1] * this->values[2][2] - this->values[1][2] * this->values[2][1]) -
				   this->values[0][1] * (this->values[1][0] * this->values[2][2] - this->values[1][2] * this->values[2][0]) +
				   this->values[0][2] * (this->values[1][0] * this->values[2][1] - this->values[1][1] * this->values[2][0]);
		} else {
			Matrix<M, M, P> m = *this; // Copy
			float det_mult = 1; // Determinant multiplier

			// Computes an upper triangular matrix
			for (uint8_t j = 0; j < M; ++j) { // Over column index. First row doesn't interest us.

				float diag = m(j, j); // Cast onto float
				if (diag != 0) { // If the diagonal element isn't 0, we'll try zeroing out the values below
					for (uint8_t i = j + 1; i < M; ++i) { // Over row index, but only to the middle
						if (m(i, j) == 0 && diag == 0) {return 0;} // Matrix is not linearly independent.
						if (m(i, j) == 0) {continue;} // Skip. We're going to include this later (pivot).

						const float row_mult = m(i, j) / diag;
						for (uint8_t k = 0; k < M; ++k) { // Substract from the whole row.
							m(i, k) -= row_mult * m(j, k);
						}
					}

				} else if (j < M) { // Diagonal is 0 - shift it to our place, unless we're at the last row

					for (uint8_t k = 0; k < N; ++k) {
						P val = m(j, k);
						m(j, k) = m(j + 1, k);
						m(j+1, k) = val;

					}
					// We performed a pivot operation, determinant is now negative.
					det_mult *= -1;
				} else { // Last row was zero'ed out. Matrix not linearly independent.
					return 0;
				}
			}

			// We have an upper triangular matrix, now actually compute the determinant
			float det = 1;
			for (uint8_t i = 0; i < M; ++i) {
				det *= m(i, i);
			}
			det *= det_mult;
			return det;
		}
	}
};
static_assert(std::is_trivially_copyable_v<Matrix<2, 2, float>>);

/// Obsolete name for Matrix
template<uint8_t M_, uint8_t N_, Arithmetic P>
using Mat = Matrix<M_, N_, P>; // Backwards compat, obsolete name

#define SOURCEPP_MAT_DEFINE(M, N) \
	template<Arithmetic P> \
	using Mat##M##x##N       = Mat<M, N, P>; \
	using Mat##M##x##N##i8   = Mat##M##x##N<int8_t>; \
	using Mat##M##x##N##i16  = Mat##M##x##N<int16_t>; \
	using Mat##M##x##N##i32  = Mat##M##x##N<int32_t>; \
	using Mat##M##x##N##i64  = Mat##M##x##N<int64_t>; \
	using Mat##M##x##N##i    = Mat##M##x##N##i32; \
	using Mat##M##x##N##ui8  = Mat##M##x##N<uint8_t>; \
	using Mat##M##x##N##ui16 = Mat##M##x##N<uint16_t>; \
	using Mat##M##x##N##ui32 = Mat##M##x##N<uint32_t>; \
	using Mat##M##x##N##ui64 = Mat##M##x##N<uint64_t>; \
	using Mat##M##x##N##ui   = Mat##M##x##N##ui32; \
	using Mat##M##x##N##f16  = Mat##M##x##N<half>; \
	using Mat##M##x##N##f32  = Mat##M##x##N<float>; \
	using Mat##M##x##N##f64  = Mat##M##x##N<double>; \
	using Mat##M##x##N##f    = Mat##M##x##N##f32

SOURCEPP_MAT_DEFINE(2, 2);
SOURCEPP_MAT_DEFINE(3, 3);
SOURCEPP_MAT_DEFINE(4, 4);
SOURCEPP_MAT_DEFINE(2, 3);
SOURCEPP_MAT_DEFINE(3, 2);
SOURCEPP_MAT_DEFINE(2, 4);
SOURCEPP_MAT_DEFINE(4, 2);
SOURCEPP_MAT_DEFINE(3, 4);
SOURCEPP_MAT_DEFINE(4, 3);

#undef SOURCEPP_MAT_DEFINE

}