#pragma once

#ifndef ENABLE_SIMD

#include "./details/vectors/vec2.h"
#include "./details/vectors/vec3.h"
#include "./details/vectors/vec4.h"

#else

#include "./simd/vec2f.h"
#include "./simd/vec3f.h"
#include "./simd/vec4f.h"

#include "./simd/vec4ui.h"

#endif // !ENABLE_SIMD

template <typename T>
inline T normalize(const T& v)
{
    return v / magnitude(v);
}

typedef vec2<float> vec2f;
typedef vec3<float> vec3f;
typedef vec4<float> vec4f;

typedef vec4<unsigned int> vec4ui;
