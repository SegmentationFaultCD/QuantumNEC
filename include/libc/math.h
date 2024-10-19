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
constexpr auto PI { 3.1415926 };
#else
#ifndef PI
#define PI 3.1415926;
#endif
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