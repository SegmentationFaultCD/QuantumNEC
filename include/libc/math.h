#pragma once
#include <stdint.h>
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef CONST
#define CONST const
#endif
#ifdef __cplusplus
extern "C" {
constexpr auto PI { 3.14159265358979323846264338327950288 };
constexpr auto F32_EPSILON { 1e-5f };
constexpr auto F64_EPSILON { 1e-10 };
constexpr auto E { 2.718281828459045235360287 };
// constexpr auto I { 1.i };
#else
#define F32_EPSILON 1e-5f
#define F64_EPSILON 1e-10
#define PI 3.14159265358979323846264338327950288
#define E 2.718281828459045235360287
#define I 1.i
#endif

float min( IN OUT float a, IN OUT float b );
float max( IN OUT float a, IN OUT float b );
float abs( IN float digit );
float pow( IN CONST int32_t &__base, IN uint64_t exp );
float floor( IN float x );
float ceil( IN float x );
float round( IN float x );
float fmod( IN float x, IN float y );
float atan( IN float x );
float atan2( IN float x, IN float y );
float utof( IN uint32_t x );
float itof( IN int32_t x );
float ulltof( uint64_t x );
float lltof( int64_t x );
#ifdef __cplusplus
}
#endif