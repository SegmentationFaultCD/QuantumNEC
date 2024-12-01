#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
constexpr auto PI { 3.14159265358979323846264338327950288 };
constexpr auto F32_EPSILON { 1e-5f };
constexpr auto F64_EPSILON { 1e-10 };
constexpr auto E { 2.718281828459045235360287 };

#else
#define F32_EPSILON 1e-5f
#define F64_EPSILON 1e-10
#define PI 3.14159265358979323846264338327950288
#define E 2.718281828459045235360287

#endif

float min( float a, float b );
float max( float a, float b );
float abs( float digit );
float pow( const int32_t &__base, uint64_t exp );
float floor( float x );
float ceil( float x );
float round( float x );
float fmod( float x, float y );
float atan( float x );
float atan2( float x, float y );
float utof( uint32_t x );
float itof( int32_t x );
float ulltof( uint64_t x );
float lltof( int64_t x );
#ifdef __cplusplus
}
#endif