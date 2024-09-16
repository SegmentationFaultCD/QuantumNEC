#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Architecture {
    Graphics::Graphics(
        IN CONST Lib::Types::Ptr< Lib::Types::BootConfig > bootConfig ) noexcept :
        config { &bootConfig->graphics_data } {
        this->window_initialize( );
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Initialize the Device Driver." << Lib::IO::endl;
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Initialize the Graphical Management." << Lib::IO::endl;
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::SYSTEM ] << "The graphical interface is ready." << Lib::IO::endl;
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the Graphical Management." << Lib::IO::endl;
    }
    Graphics::~Graphics( VOID ) noexcept {
    }
    auto Graphics::display_pixel(     // 填充一个像素
        IN Lib::Types::Ptr< Lib::Types::uint32_t > video_handle,
        IN Lib::Types::L_Ref< Lib::IO::DisplayColor > color,
        IN Lib::Types::L_Ref< Lib::Types::int32_t > X,
        IN Lib::Types::L_Ref< Lib::Types::int32_t > Y,
        IN Lib::Types::L_Ref< Lib::Types::uint32_t > video_long ) noexcept -> Lib::Types::SystemStatus {
        auto pixel { video_handle };
        pixel += Y * video_long + X;
        *pixel = static_cast< Lib::Types::uint32_t >( color );
        return SYSTEM_SUCCESS;
    }

    auto Graphics::set_pos(
        IN Lib::Types::L_Ref< Lib::Types::uint32_t > XResolution,
        IN Lib::Types::L_Ref< Lib::Types::uint32_t > YResolution,
        IN Lib::Types::int32_t XPosition, IN Lib::Types::int32_t YPosition,
        IN Lib::Types::int32_t XCharSize, IN Lib::Types::int32_t YCharSize,
        IN Lib::Types::int64_t column, IN Lib::Types::uint64_t row,
        IN Lib::Types::Ptr< Lib::Types::uint32_t > FB_addr,
        IN Lib::Types::uint64_t FB_length ) noexcept -> Lib::Types::SystemStatus {
        // 设置Pos结构体
        Lib::IO::Position Pos { };
        Pos.XResolution = static_cast< Lib::Types::int32_t >( XResolution );
        Pos.YResolution = static_cast< Lib::Types::int32_t >( YResolution );
        Pos.XPosition = XPosition;
        Pos.YPosition = YPosition;
        Pos.XCharSize = XCharSize;
        Pos.YCharSize = YCharSize;
        Pos.FB_addr = FB_addr;
        Pos.FB_length = FB_length;
        Pos.column = column;
        Pos.row = row;
        // 初始化系统标准输出函数
        Lib::IO::sout.getPos( Pos );

        return SYSTEM_SUCCESS;
    }
    // 窗口初始化
    auto Graphics::window_initialize( VOID ) noexcept -> Lib::Types::SystemStatus {
        Lib::Types::SystemStatus Status { SYSTEM_SUCCESS };
        Status = this->set_pos( this->config->HorizontalResolution,
                                this->config->VerticalResolution,
                                0,      // 开始的长度
                                0,      // 开始的高度
                                8,      // 字符长度
                                16,     // 字符高度
                                -1,     // \n列数
                                0,      // \n排数
                                reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint32_t > >(
                                    this->config->FrameBufferBase ),
                                this->config->HorizontalResolution
                                    * this->config->VerticalResolution * 4 );

        this->display_fill( 0, 0, this->config->HorizontalResolution,
                            this->config->VerticalResolution,
                            Lib::IO::DisplayColor::BLACK );
        return Status;
    }

    auto Graphics::GUI_initialize( VOID ) noexcept -> Lib::Types::SystemStatus {
        Lib::Types::SystemStatus Status { SYSTEM_SUCCESS };
        Status = this->set_pos( this->config->HorizontalResolution,
                                this->config->VerticalResolution,
                                1,      // 开始的长度
                                2,      // 开始的高度
                                8,      // 字符长度
                                16,     // 字符高度
                                1,      // \n列数
                                0,      // \n排数
                                reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint32_t > >(
                                    this->config->FrameBufferBase ),
                                this->config->HorizontalResolution
                                    * this->config->VerticalResolution * 4 );
        this->display_fill( 0, 0, this->config->HorizontalResolution,
                            this->config->VerticalResolution,
                            Lib::IO::DisplayColor::GRAYISH );
        return Status;
    }

    auto Graphics::display_shell( VOID ) noexcept -> Lib::Types::SystemStatus {
        Lib::Types::SystemStatus Status { SYSTEM_SUCCESS };

        // 第一层
        this->display_fill( 12, 12, 716, this->config->VerticalResolution, Lib::IO::DisplayColor::WHITE );
        // 第二层
        this->display_fill( 14, 28, 714, this->config->VerticalResolution - 4, Lib::IO::DisplayColor::GRAY );
        // 第三层
        this->display_fill( 16, 30, 712, this->config->VerticalResolution - 8, Lib::IO::DisplayColor::BLACK );
        return Status;
    }
    auto Graphics::display_error_blue_screen( IN CONST Lib::Types::uint8_t irq ) noexcept -> Lib::Types::SystemStatus {
        if ( irq >= 0x20 ) {     // 如果是中断就没有必要画了
            return SYSTEM_SUCCESS;
        }
        Lib::Types::SystemStatus Status { SYSTEM_SUCCESS };
        Status = this->set_pos( this->config->HorizontalResolution,
                                this->config->VerticalResolution,
                                1,      // 开始的长度
                                1,      // 开始的高度
                                8,      // 字符长度
                                16,     // 字符高度
                                0,      // \n列数
                                0,      // \n排数
                                reinterpret_cast< Lib::Types::uint32_t far * >(
                                    this->config->FrameBufferBase ),
                                this->config->HorizontalResolution
                                    * this->config->VerticalResolution * 4 );

        // 填充背景
        this->display_fill( 0, 0, this->config->HorizontalResolution,
                            this->config->VerticalResolution,
                            Lib::IO::DisplayColor::WHITE );
        this->display_fill( 8, 8, this->config->HorizontalResolution - 8,
                            this->config->VerticalResolution - 8,
                            Lib::IO::DisplayColor::BLUE );
        return Status;
    }
    auto Graphics::display_fill(
        IN CONST Lib::Types::int32_t x0, IN CONST Lib::Types::int32_t y0,
        IN CONST Lib::Types::int32_t x1, IN CONST Lib::Types::int32_t y1,
        IN Lib::Types::R_Ref< Lib::IO::DisplayColor > color )
        ->Lib::Types::SystemStatus {
        Lib::Types::SystemStatus Status { SYSTEM_SUCCESS };
        auto dark_color { color };
        auto video_handle { reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint32_t > >( this->config->FrameBufferBase ) };
        for ( auto y { y0 }; y < y1; ++y )
        // 按照坐标从x0 -> x1 y0 -> y1依次填充像素
        {
            for ( auto x { x0 }; x < x1; ++x ) {
                Status = this->display_pixel( video_handle, dark_color, x, y,
                                              this->config->HorizontalResolution );
            }
        }
        return Status;
    }
}