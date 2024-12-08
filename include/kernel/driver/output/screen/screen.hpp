#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {

class Screen {
public:
    explicit Screen( void ) noexcept;
    virtual ~Screen( void ) noexcept;

private:
    /**
     * @brief 绘制像素点
     * @param video_handle 要写入的地址
     * @param color 写入像素点的颜色
     * @param X 横轴指标
     * @param Y 纵轴指标
     * @param video_long 视图长度
     */
    auto display_pixel( IN uint64_t *video_handle,
                        IN int32_t   color,
                        IN int64_t   X,
                        IN int64_t   Y,
                        IN uint64_t  video_long ) noexcept -> void;
    /**
     * @brief 设置Pos参数
     * @param XResolution 横轴长度
     * @param YResolution 纵轴高度
     * @param XPosition 开始打印的地址-横轴
     * @param YPosition 开始打印的地址-纵轴
     * @param XCharSize 字符长度
     * @param YCharSize 字符高度
     * @param column \n列数
     * @param row \n排数
     * @param FB_addr 起始地址
     * @param FB_length 字的长度
     */
    auto set_pos(
        IN uint64_t XResolution,
        IN uint64_t YResolution,
        IN int64_t XPosition, IN int64_t YPosition,
        IN int64_t XCharSize, IN int64_t YCharSize,
        IN int64_t column, IN uint64_t row,
        IN uint64_t *FB_addr,
        IN uint64_t  FB_length ) noexcept -> void;

public:
    /**
     * @brief 绘制方形
     * @param x0 左上角点坐标x
     * @param y0 左上角点坐标y
     * @param x1 右下角点坐标x
     * @param y1 右下角点坐标y
     * @param color 填充颜色
     */
    auto display_fill(
        IN int64_t x0, IN int64_t y0,
        IN int64_t x1, IN int64_t y1,
        IN int32_t color ) noexcept -> void;
    ;

private:
    limine_framebuffer *frame;
};
}     // namespace QuantumNEC::Kernel
