#pragma once
#include <kernel/driver/driver.hpp>
#include <lib/Uefi.hpp>
#include <lib/spin_lock.hpp>
#include <lib/utils.hpp>
#include <libcxx/format.hpp>
#include <libcxx/iostream.hpp>
PUBLIC namespace QuantumNEC {
    PUBLIC constexpr auto ZEROPAD { 1 };
    PUBLIC constexpr auto SIGN { 2 };
    PUBLIC constexpr auto PLUS { 4 };
    PUBLIC constexpr auto SPACE { 8 };
    PUBLIC constexpr auto LEFT { 16 };
    PUBLIC constexpr auto SPECIAL { 32 };
    PUBLIC constexpr auto SMALL { 64 };
    PUBLIC constexpr auto LINEEOF = 0;
    /**
     * @brief 显示颜色
     */
    PUBLIC enum DisplayColor : int {
        WHITE      = 0x00ffffff,     // 白
        BLACK      = 0x00000000,     // 黑
        RED        = 0x00ff0000,     // 红
        ORANGE     = 0x00ff8000,     // 橙
        YELLOW     = 0x00ffff00,     // 黄
        GREEN      = 0x0000ff00,     // 绿
        BLUE       = 0x000040ac,     // 蓝
        INDIGO     = 0x0000ffff,     // 青
        PURPLE     = 0x008000ff,     // 紫
        GRAY       = 0x00a4a4a4,     // 灰
        BLUEGRAY   = 0x005486ab,     // 蓝灰
        YELLOWGRAY = 0x009da682,     // 黄灰
        INDIGOGRAY = 0x00b5eaea,     // 青灰
        GRAYISH    = 0x008d8d8d      // 淡灰
    };

    PUBLIC struct Position {
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
        explicit Position( VOID ) noexcept = default;
    };

    /**
     * @brief 输出日志
     * @param FRcolor 前景色
     * @param BKcolor 背景色
     * @param fmt 格式字符串
     * @param args 参数
     */
    PUBLIC auto            printk( IN DisplayColor FRcolor, IN DisplayColor BKcolor, IN CONST char_t * fmt, IN... ) -> VOID;
    PUBLIC inline char_t   buffer[ 0x10000 ] { };     // 64kb缓冲区
    PUBLIC inline Position Pos { };

    /**
     * @brief 打印字符
     * @param FB 起始地址
     * @param Xsize 字符长度
     * @param X 起始x轴
     * @param Y 起始y轴
     * @param FRcolor 前景色
     * @param BKcolor 背景色
     * @param Font 字体
     */
    auto putc(
        IN uint64_t * FB,
        IN int64_t Xsize, IN int64_t X,
        IN int64_t Y, IN DisplayColor FRcolor,
        IN DisplayColor BKcolor, IN uchar_t Font )
        -> VOID;
    inline Lib::SpinLock pri_lock { };
    template < typename... Args >
    PUBLIC auto print_( IN std::format_string< std::type_identity_t< Args >... > fmt, IN Args && ...args ) -> VOID {
        auto fmt_str = format( fmt, std::forward< Args >( args )... );

        while ( *fmt_str ) {
            switch ( *fmt_str ) {
            case '\a': break;
            case '\n':
                Pos.YPosition++;
                Pos.XPosition = Pos.column;     // 如果是，将光标行数加1, 列数设为BasePrint::Pos->column
                Kernel::Output::write( '\n' );
                break;
            case '\t':
                for ( auto i { 0 }; i < 4; ++i ) {
                    putc( Pos.FB_addr, Pos.XResolution,
                          Pos.XPosition * Pos.XCharSize,
                          Pos.YPosition * Pos.YCharSize, DisplayColor::WHITE,
                          DisplayColor::BLACK, ' ' );
                    Kernel::Output::write( ' ' );
                    ++Pos.XPosition;
                }

                break;
            case '\r':
                Pos.XPosition = Pos.column + 1;
                break;
            case '\b':
                Pos.XPosition--;
                if ( Pos.XPosition < LINEEOF ) {
                    Pos.XPosition = ( Pos.XResolution / Pos.XCharSize - 1 )
                                    * Pos.XCharSize;
                    Pos.YPosition--;
                    if ( Pos.YPosition < LINEEOF ) {
                        Pos.YPosition = ( Pos.YResolution / Pos.YCharSize )
                                        * Pos.YCharSize;
                    }
                }
                putc( Pos.FB_addr, Pos.XResolution,
                      Pos.XPosition * Pos.XCharSize,
                      Pos.YPosition * Pos.YCharSize, DisplayColor::WHITE,
                      DisplayColor::BLACK, ' ' );
                Kernel::Output::write( ' ' );
                break;
            default:
                putc( Pos.FB_addr, Pos.XResolution,
                      Pos.XPosition * Pos.XCharSize,
                      Pos.YPosition * Pos.YCharSize, DisplayColor::WHITE,
                      DisplayColor::BLACK, *fmt_str );
                ++Pos.XPosition;
                Kernel::Output::write( *fmt_str );
            }

            // 结尾部分
            if ( Pos.XPosition
                 >= ( Pos.XResolution / Pos.XCharSize ) ) {
                ++( Pos.YPosition );
                Pos.XPosition = LINEEOF;
            }
            if ( Pos.YPosition
                 >= ( Pos.YResolution / Pos.YCharSize ) ) {
                Pos.YPosition = LINEEOF;
            }
            fmt_str++;
        }
    }
    template < std::ostream::HeadLevel level, typename... Args >
    PUBLIC auto print( IN const char *fmt, IN Args &&...args ) -> VOID {
        constexpr auto level_table = [ & ] consteval -> std::tuple< const char *, DisplayColor, DisplayColor > {
            using enum std::ostream::HeadLevel;
            using enum DisplayColor;
            const char_t *level_string { };
            DisplayColor  fcolor, bcolor;
            switch ( level ) {
            case SYSTEM:
                level_string = "[ SYSTEM ]   ";
                fcolor       = BLUEGRAY;
                bcolor       = BLACK;
                break;
            case OK:
                level_string = "[ OK/READY ] ";
                fcolor       = GREEN;
                bcolor       = BLACK;
                break;
            case INFO:
                level_string = "[ INFO ]     ";
                fcolor       = INDIGO;
                bcolor       = BLACK;
                break;
            case START:
                level_string = "[ START ]    ";
                fcolor       = YELLOW;
                bcolor       = BLACK;
                break;
            case ERROR:
                level_string = "[ ERROR ]    ";
                fcolor       = RED;
                bcolor       = BLACK;
                break;
            case DEBUG:
                level_string = "[ DEBUG ]    ";
                fcolor       = ORANGE;
                bcolor       = BLACK;
                break;
            case WARNING:
                level_string = "[ WARNING ]  ";
                fcolor       = YELLOW;
                bcolor       = BLACK;
                break;
            }
            return { level_string, fcolor, bcolor };
        };
        auto &&[ level_str, _fcolor, _bcolor ] = level_table( );

        while ( *level_str ) {
            if ( *level_str == '[' || *level_str == ']' ) {
                putc( Pos.FB_addr, Pos.XResolution,
                      Pos.XPosition * Pos.XCharSize,
                      Pos.YPosition * Pos.YCharSize, DisplayColor::WHITE,
                      _bcolor, *level_str );
                Kernel::Output::write( *level_str );
            }
            else {
                putc( Pos.FB_addr, Pos.XResolution,
                      Pos.XPosition * Pos.XCharSize,
                      Pos.YPosition * Pos.YCharSize, _fcolor,
                      _bcolor, *level_str );
                Kernel::Output::write( *level_str );
            }
            ++( Pos.XPosition );
            level_str++;
        }
        print_( fmt, args... );
    }

    template < std::ostream::HeadLevel level, typename... Args >
    auto println( IN const char *fmt, IN Args &&...args ) {
        pri_lock.acquire( );
        print< level >( fmt, std::forward< Args >( args )... );
        Pos.YPosition++;
        Pos.XPosition = Pos.column;
        Kernel::Output::write( '\n' );
        pri_lock.release( );
    }
    template < typename... Args >
    auto println( IN std::format_string< std::type_identity_t< Args >... > fmt, IN Args && ...args ) {
        pri_lock.acquire( );
        print_( fmt, std::forward< Args >( args )... );
        Pos.YPosition++;
        Pos.XPosition = Pos.column;
        Kernel::Output::write( '\n' );
        pri_lock.release( );
    }
}
