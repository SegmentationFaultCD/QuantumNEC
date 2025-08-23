#pragma once
#include <cstdint>
#include <fmt/color.h>
#include <kernel/driver/serial_port/serial_port.hpp>
#include <kernel/task/lock/spinlock.hpp>
#include <lib/format.hpp>
#include <limine.h>
namespace Display {

constexpr auto LINEEOF = 0;
auto putc( std::uint64_t *FB, std::int64_t Xsize, std::int64_t X, std::int64_t Y, char16_t font ) -> void;

struct _Pos_ {
    int64_t XResolution;
    int64_t YResolution;
    int64_t XPosition;
    int64_t YPosition;
    int64_t XCharSize;
    int64_t YCharSize;
    uint64_t *FB_addr;
    uint64_t FB_length;
    int64_t column;
    uint64_t row;
} inline position;

inline auto initialize( limine_framebuffer *frame ) -> void {
    position.XResolution = static_cast< int64_t >( frame->width );
    position.YResolution = static_cast< int64_t >( frame->height );
    position.XPosition = 0;
    position.YPosition = 0;
    position.XCharSize = 8;
    position.YCharSize = 16;
    position.FB_addr = (uint64_t *)frame->address;
    position.FB_length = frame->width * frame->height * 4;
    position.column = 0;
    position.row = 0;
}

template < typename... Args >
auto print( fmt::format_string< Args... > fmt, Args... args ) {
    Driver::SerialPort output;

    auto fmt_str = Library::format( fmt, args... );

    for ( auto ch : fmt_str ) {
        switch ( ch ) {
        case '\a':
            break;
        case '\n':
            position.YPosition++;
            position.XPosition = position.column;     // 如果是，将光标行数加1, 列数设为BasePrint::Pos->column
            output.write( '\n' );
            break;
        case '\t':
            for ( auto i { 0 }; i < 4; ++i ) {
                putc( position.FB_addr, position.XResolution,
                      position.XPosition * position.XCharSize,
                      position.YPosition * position.YCharSize, ' ' );
                output.write( ' ' );
                ++position.XPosition;
            }
            break;
        case '\r':
            position.XPosition = position.column + 1;
            break;
        case '\b':
            position.XPosition--;
            if ( position.XPosition < LINEEOF ) {
                position.XPosition = ( position.XResolution / position.XCharSize - 1 )
                                     * position.XCharSize;
                position.YPosition--;
                if ( position.YPosition < LINEEOF ) {
                    position.YPosition = ( position.YResolution / position.YCharSize )
                                         * position.YCharSize;
                }
            }
            putc( position.FB_addr, position.XResolution,
                  position.XPosition * position.XCharSize,
                  position.YPosition * position.YCharSize, ' ' );
            output.write( ' ' );
            break;
        default:
            putc( position.FB_addr, position.XResolution,
                  position.XPosition * position.XCharSize,
                  position.YPosition * position.YCharSize, ch );
            ++position.XPosition;
            output.write( ch );
        }
        // 结尾部分
        if ( position.XPosition >= ( position.XResolution / position.XCharSize ) ) {
            ++( position.YPosition );
            position.XPosition = LINEEOF;
        }
        if ( position.YPosition >= ( position.YResolution / position.YCharSize ) ) {
            position.YPosition = LINEEOF;
        }
    }
}

template < typename... Args >
auto println( fmt::format_string< Args... > fmt, Args... args ) {
    Display::print( fmt, args... );
    Display::print( "\n" );
}

inline auto println( ) -> void {
    print( "\n" );
}
}     // namespace Display