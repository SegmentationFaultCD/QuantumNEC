#pragma once
#include <Lib/Types/Uefi.hpp>
#include <cstdarg>
PUBLIC namespace QuantumNEC::Utils {
    /**
     *
     */
    PUBLIC auto skip_atoi(
        IN CONST Lib::Types::Ptr< Lib::Types::Ptr< CONST Lib::Types::char_t > > str )
        ->Lib::Types::size_t;
    /**
     *
     */
    PUBLIC auto number( IN OUT Lib::Types::Ptr< Lib::Types::char_t > str,
                        IN Lib::Types::int64_t num, IN Lib::Types::int32_t base,
                        IN Lib::Types::int32_t size,
                        IN Lib::Types::int32_t precision,
                        IN Lib::Types::int32_t type )
        ->Lib::Types::Ptr< Lib::Types::char_t >;
    /**
     *
     */
    PUBLIC auto vsprintf( IN Lib::Types::Ptr< Lib::Types::char_t > buf,
                          IN Lib::Types::Ptr< CONST Lib::Types::char_t > fmt,
                          va_list args )
        ->Lib::Types::int32_t;
    /**
     * @brief 得到颜色
     * @param r 红
     * @param g 绿
     * @param b 蓝
     */
    PUBLIC constexpr inline auto RGB( Lib::Types::uint8_t r, Lib::Types::uint8_t g,
                                      Lib::Types::uint8_t b )
        ->Lib::Types::uint32_t {
        return ( ( r * 65536 ) + ( g * 256 ) + b );
    }
}