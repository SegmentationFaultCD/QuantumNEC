#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/output/screen/screen.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel {
using namespace Lib;
using namespace std;

Screen::Screen( void ) noexcept {
    frame = &__config.graphics_data;

    this->set_pos( frame->width,
                   frame->height,
                   0,      // 开始的长度
                   0,      // 开始的高度
                   8,      // 字符长度
                   16,     // 字符高度
                   0,      // \n列数
                   0,      // \n排数
                   reinterpret_cast< uint64_t * >( frame->address ),
                   frame->width * frame->height * 4 );

    this->display_fill( 0, 0, frame->width, frame->height, DisplayColor::BLACK );
}
Screen::~Screen( void ) noexcept {
}
auto Screen::display_pixel(     // 填充一个像素
    IN uint64_t *video_handle,
    IN int32_t   color,
    IN int64_t   X,
    IN int64_t   Y,
    IN uint64_t  video_long ) noexcept -> void {
    auto *pixel { (uint32_t *)video_handle };
    pixel += Y * video_long + X;
    *pixel = static_cast< uint32_t >( color );
}

auto Screen::set_pos(
    IN uint64_t XResolution,
    IN uint64_t YResolution,
    IN int64_t XPosition, IN int64_t YPosition,
    IN int64_t XCharSize, IN int64_t YCharSize,
    IN int64_t column, IN uint64_t row,
    IN uint64_t *FB_addr,
    IN uint64_t  FB_length ) noexcept -> void {
    // 设置Pos结构体

    Pos.XResolution = static_cast< int64_t >( XResolution );
    Pos.YResolution = static_cast< int64_t >( YResolution );
    Pos.XPosition   = XPosition;
    Pos.YPosition   = YPosition;
    Pos.XCharSize   = XCharSize;
    Pos.YCharSize   = YCharSize;
    Pos.FB_addr     = FB_addr;
    Pos.FB_length   = FB_length;
    Pos.column      = column;
    Pos.row         = row;
    // 初始化系统标准输出函数
}

auto Screen::display_fill(
    IN int64_t x0, IN int64_t y0,
    IN int64_t x1, IN int64_t y1,
    IN int32_t color ) noexcept
    -> void {
    auto dark_color { color };
    auto video_handle { reinterpret_cast< uint64_t * >( frame->address ) };
    for ( auto y { y0 }; y < y1; ++y )
    // 按照坐标从x0 -> x1 y0 -> y1依次填充像素
    {
        for ( auto x { x0 }; x < x1; ++x ) {
            this->display_pixel( video_handle, dark_color, x, y,
                                 frame->width );
        }
    }
}
}     // namespace QuantumNEC::Kernel
