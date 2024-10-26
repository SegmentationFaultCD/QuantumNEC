#pragma once
#include <lib/Uefi.hpp>
#include <cstdarg>
PUBLIC namespace QuantumNEC::Lib {
    PUBLIC constexpr CONST auto ZEROPAD { 1 };
    PUBLIC constexpr CONST auto SIGN { 2 };
    PUBLIC constexpr CONST auto PLUS { 4 };
    PUBLIC constexpr CONST auto SPACE { 8 };
    PUBLIC constexpr CONST auto LEFT { 16 };
    PUBLIC constexpr CONST auto SPECIAL { 32 };
    PUBLIC constexpr CONST auto SMALL { 64 };
    /**
     *
     */
    PUBLIC auto skip_atoi( IN CONST char_t * *str )
        -> size_t;
    /**
     *
     */
    PUBLIC auto number(
        IN OUT char_t * str,
        IN int64_t num, IN int32_t base,
        IN int32_t size,
        IN int32_t precision,
        IN int32_t type )
        -> char_t *;
    /**
     *
     */
    PUBLIC auto vsprintf( IN char_t * buf,
                          IN CONST char_t * fmt,
                          va_list args )
        -> int32_t;
    /**
     * @brief 得到颜色
     * @param r 红
     * @param g 绿
     * @param b 蓝
     */
    PUBLIC constexpr inline auto RGB( IN uint8_t r, IN uint8_t g, IN uint8_t b ) {
        return ( ( r * 65536 ) + ( g * 256 ) + b );
    }
}