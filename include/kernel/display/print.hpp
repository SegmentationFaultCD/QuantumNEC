#pragma once
#include <cstdint>
#include <kernel/task/lock/spinlock.hpp>
#include <limine.h>
namespace Display {
inline std::uint8_t ascii[ 256 ][ 16 ];

inline auto putc( std::uint64_t *FB, std::int64_t Xsize, std::int64_t X, std::int64_t Y, char16_t font ) -> void {
    std::int32_t   i { }, j { };
    std::uint32_t *Address { };
    std::uint8_t  *FontPtr { ascii[ font ] };
    std::int32_t   testval { };
    for ( i = 0; i < 16; i++ ) {
        Address = (uint32_t *)( FB ) + Xsize * ( Y + i ) + X;
        testval = 0x100;
        for ( j = 0; j < 8; j++ ) {
            testval = testval >> 1;
            if ( *FontPtr & testval )
                *Address = 0x00ffffffu;
            else
                *Address = 0x00000000u;
            Address++;
        }
        FontPtr++;
    }
}

inline struct {
    int64_t   XResolution;
    int64_t   YResolution;
    int64_t   XPosition;
    int64_t   YPosition;
    int64_t   XCharSize;
    int64_t   YCharSize;
    uint64_t *FB_addr;
    uint64_t  FB_length;
    int64_t   column;
    uint64_t  row;
} position;

inline auto initialize( limine_framebuffer *frame ) {
    position.XResolution = static_cast< int64_t >( frame->width );
    position.YResolution = static_cast< int64_t >( frame->height );
    position.XPosition   = 0;
    position.YPosition   = 0;
    position.XCharSize   = 8;
    position.YCharSize   = 16;
    position.FB_addr     = (uint64_t *)frame->address;
    position.FB_length   = frame->width * frame->height * 4;
    position.column      = 0;
    position.row         = 0;
}

}     // namespace Display