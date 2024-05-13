#pragma once

#include <intrin.h>

namespace harmony
{
	namespace SSE
	{
		class float4
		{
		public:
			union
			{
				struct { float x, y, z, w; };
				struct { float r, g, b, a; };
				struct { float s, t, p, q; };
				__m128 vec;
			};

			float4()
				: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

			float4(const float s)
				: x(s), y(s), z(s), w(s) {}

			float4(const __m128 v)
				: vec(v) {}

			inline void increment()
			{
				_mm_add_ps(vec, float4(1.0f).vec);
			}

			inline void decrement()
			{
				_mm_sub_ps(vec, float4(1.0f).vec);
			}
		};

		inline float4 add(const float4& vec1, const float4& vec2)
		{
			return float4(_mm_add_ps(vec1.vec, vec2.vec));
		}
		inline float4 sub(const float4& vec1, const float4& vec2)
		{
			return float4(_mm_sub_ps(vec1.vec, vec2.vec));
		}
		inline float4 mul(const float4& vec1, const float4& vec2)
		{
			return float4(_mm_mul_ps(vec1.vec, vec2.vec));
		}
		inline float4 div(const float4& vec1, const float4& vec2)
		{
			return float4(_mm_div_ps(vec1.vec, vec2.vec));
		}
	}

	namespace SSE
	{
		class uint4
		{
		public:
			union
			{
				struct { unsigned int x, y, z, w; };
				struct { unsigned int r, g, b, a; };
				struct { unsigned int s, t, p, q; };
				__m128i vec;
			};

			uint4()
				: x(0), y(0), z(0), w(0)
			{}

			uint4(const unsigned int s)
				: x(s), y(s), z(s), w(s)
			{}

			uint4(const __m128i v)
				: vec(v)
			{}

			inline void increment()
			{
				_mm_add_epi32(vec, uint4(1.0f).vec);
			}

			inline void decrement()
			{
				_mm_sub_epi32(vec, uint4(1.0f).vec);
			}
		};

		inline uint4 add(const uint4& vec1, const uint4& vec2)
		{
			return uint4(_mm_add_epi32(vec1.vec, vec2.vec));
		}
		inline uint4 sub(const uint4& vec1, const uint4& vec2)
		{
			return uint4(_mm_sub_epi32(vec1.vec, vec2.vec));
		}
		inline uint4 mul(const uint4& vec1, const uint4& vec2)
		{
			return uint4(_mm_mul_epi32(vec1.vec, vec2.vec));
		}
		inline uint4 div(const uint4& vec1, const uint4& vec2)
		{
			return uint4(_mm_div_epi32(vec1.vec, vec2.vec));
		}
	}
}
