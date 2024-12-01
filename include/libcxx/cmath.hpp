#pragma once

#include <cstdint>
#include <stdfloat>
namespace std {
constexpr auto PI { 3.1415926 };
extern "C" {
auto min( float a, float b ) -> float;
auto max( float a, float b ) -> float;
auto abs( float digit ) -> float;
auto pow( int32_t &__base, uint64_t exp ) -> float;
auto floor( float x ) -> float;
auto ceil( float x ) -> float;
auto round( float x ) -> float;
auto fmod( float x, float y ) -> float;
auto atan( float x ) -> float;
auto atan2( float x, float y ) -> float;
auto utof( uint32_t x ) -> float;
auto itof( int32_t x ) -> float;
auto ulltof( uint64_t x ) -> float;
auto lltof( int64_t x ) -> float;
}
}     // namespace std