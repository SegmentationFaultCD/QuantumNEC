#include <Lib/IO/Base/base_print.hpp>
#include <Lib/IO/Base/font>
#include <Lib/Types/type_file.hpp>
#include <Arch/Arch.hpp>
PUBLIC namespace QuantumNEC::Lib::IO {
    BasePrint::BasePrint( IN Lib::Types::L_Ref< CONST Position > _Pos ) noexcept :
        Pos { _Pos } {
    }
    auto BasePrint::getPos( IN Lib::Types::L_Ref< CONST Position > Pos ) CONST->VOID {
        this->Pos = Pos;
    }
    auto BasePrint::putPos( VOID ) CONST->Lib::Types::L_Ref< CONST Position > {
        return this->Pos;
    }
    auto BasePrint::putc(
        IN Lib::Types::Ptr< Lib::Types::uint32_t > FB,
        IN Lib::Types::int32_t Xsize, IN Lib::Types::int32_t X,
        IN Lib::Types::int32_t Y, IN DisplayColor FRcolor,
        IN DisplayColor BKcolor, IN Lib::Types::uchar_t Font )
        ->VOID {
        Lib::Types::int32_t i { }, j { };
        Lib::Types::Ptr< Lib::Types::uint32_t > Address { };
        Lib::Types::Ptr< Lib::Types::uint8_t > FontPtr { Lib::IO::FONT_ASCII[ Font ] };
        Lib::Types::int32_t testval { };
        for ( i = 0; i < 16; i++ ) {
            Address = FB + Xsize * ( Y + i ) + X;
            testval = 0x100;
            for ( j = 0; j < 8; j++ ) {
                testval = testval >> 1;
                if ( *FontPtr & testval )
                    *Address = static_cast< Lib::Types::uint32_t >( FRcolor );
                else
                    *Address = static_cast< Lib::Types::uint32_t >( BKcolor );
                Address++;
            }
            FontPtr++;
        }
    }
    auto BasePrint::printk(
        IN DisplayColor FRcolor, IN DisplayColor BKcolor,
        IN Lib::Types::Ptr< CONST Lib::Types::char_t > fmt,
        IN... )
        ->Lib::Types::SystemStatus {
        // 用于输出格式字符串的函数basePrintk
        Lib::Types::SystemStatus Status { SYSTEM_SUCCESS };
        Types::int32_t len { };
        Types::int32_t count { };
        Types::int32_t line = Lib::Types::LINEEOF;
        va_list args;
        va_start( args, fmt );
        len = Utils::vsprintf( buffer, fmt, args );
        va_end( args );

        for ( count = 0; count < len || line; ++count ) {
            if ( line > Lib::Types::LINEEOF ) {
                --count;
                goto L_tab;
            }
            if ( *( buffer + count ) == '\a' )     // 判断字符否为\a
            {
                Architecture::ArchitectureManager< TARGET_ARCH >::beep( );
            }
            if ( *( buffer + count ) == '\n' )     // 判断字符否为\n
            {
                this->Pos.YPosition++;
                this->Pos.XPosition = this->Pos.column;
                // 如果是，将光标行数加1, 列数设为this->Pos->column
            }
            if ( *( buffer + count ) == '\r' )     // 判断字符否为\r
            {
                this->Pos.XPosition = this->Pos.column + 1;
                // 如果是，将光标列数设置为最前
            }

            else if ( *( buffer + count ) == '\b' )     // 判断字符否为\b
            {
                this->Pos.XPosition--;
                if ( this->Pos.XPosition < Lib::Types::LINEEOF ) {
                    this->Pos.XPosition = ( this->Pos.XResolution / this->Pos.XCharSize - 1 )
                                          * this->Pos.XCharSize;
                    this->Pos.YPosition--;
                    if ( this->Pos.YPosition < Lib::Types::LINEEOF ) {
                        this->Pos.YPosition = ( this->Pos.YResolution / this->Pos.YCharSize )
                                              * this->Pos.YCharSize;
                    }
                }
                this->putc( this->Pos.FB_addr, this->Pos.XResolution,
                            this->Pos.XPosition * this->Pos.XCharSize,
                            this->Pos.YPosition * this->Pos.YCharSize, FRcolor,
                            BKcolor, ' ' );
                // 如果是, 调用putc函数打印空格来覆盖"\b", 并调整位置
            }
            else if ( *( buffer + count ) == '\t' )     // 判断字符否为\t
            {
                line = ( ( this->Pos.XPosition + 4 ) & ~( 4 - 1 ) )
                       - this->Pos.XPosition;     // 表示一个制表符需要4个字符位置
            L_tab:
                --line;
                this->putc( this->Pos.FB_addr, this->Pos.XResolution,
                            this->Pos.XPosition * this->Pos.XCharSize,
                            this->Pos.YPosition * this->Pos.YCharSize, FRcolor,
                            BKcolor, ' ' );
                ++( this->Pos.XPosition );
                // 如果是\t, 那么计算一个制表符所需要的空格数, 并填充空格
            }
            else {     // 将字符打印在屏幕上
                this->putc( this->Pos.FB_addr, this->Pos.XResolution,
                            this->Pos.XPosition * this->Pos.XCharSize,
                            this->Pos.YPosition * this->Pos.YCharSize, FRcolor,
                            BKcolor, ( *( buffer + count ) ) );
                ++( this->Pos.XPosition );
            }
            // 结尾部分
            if ( this->Pos.XPosition
                 >= ( this->Pos.XResolution / this->Pos.XCharSize ) ) {
                ++( this->Pos.YPosition );
                this->Pos.XPosition = Lib::Types::LINEEOF;
            }
            if ( this->Pos.YPosition
                 >= ( this->Pos.YResolution / this->Pos.YCharSize ) ) {
                this->Pos.YPosition = Lib::Types::LINEEOF;
            }
        }
        return Status;
    }
}