#pragma once
#include <lib/Uefi.hpp>
#include <concepts>
PUBLIC namespace std {
    using namespace QuantumNEC;
    PUBLIC constexpr auto PI { 3.1415926 };
    _C_LINK {
        PUBLIC auto min( IN OUT double32_t a, IN OUT double32_t b ) -> double32_t;
        PUBLIC auto max( IN OUT double32_t a, IN OUT double32_t b ) -> double32_t;
        /**
         * @brief 求绝对值
         * @param digit 求这个数的绝对值
         */
        PUBLIC auto abs( IN double32_t digit ) -> double32_t;
        PUBLIC auto pow( IN CONST int32_t & __base, IN uint64_t exp ) -> double32_t;
        PUBLIC auto floor( IN double32_t x ) -> double32_t;
        PUBLIC auto ceil( IN double32_t x ) -> double32_t;
        PUBLIC auto round( IN double32_t x ) -> double32_t;
        PUBLIC auto fmod( IN double32_t x, IN double32_t y ) -> double32_t;
        PUBLIC auto atan( IN double32_t x ) -> double32_t;
        PUBLIC auto atan2( IN double32_t x, IN double32_t y ) -> double32_t;
        PUBLIC auto utof( IN uint32_t x ) -> double32_t;
        PUBLIC auto itof( IN int32_t x ) -> double32_t;
        PUBLIC auto ulltof( uint64_t x ) -> double32_t;
        PUBLIC auto lltof( int64_t x ) -> double32_t;
    }
}