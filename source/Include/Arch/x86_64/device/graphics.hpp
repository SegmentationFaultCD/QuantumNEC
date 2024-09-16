#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/IO/Base/base_print.hpp>
#include <Lib/Types/type_base.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Lib/Types/type_int.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    /**
     * @brief 图形化窗口驱动管理
     */
    PUBLIC class Graphics
    {
    public:
        explicit Graphics( IN CONST Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;
        virtual ~Graphics( VOID ) noexcept;

    public:
        /**
         * @brief 图形界面初始化
         */
        auto window_initialize( VOID ) noexcept -> Lib::Types::SystemStatus;
        /**
         * @brief 桌面初始化
         */
        auto GUI_initialize( VOID ) noexcept -> Lib::Types::SystemStatus;
        /**
         * @brief 绘制终端
         */
        auto display_shell( VOID ) noexcept -> Lib::Types::SystemStatus;

    public:
        /**
         * @brief 蓝屏
         * @param irq 异常中断号
         */
        auto display_error_blue_screen( IN CONST Lib::Types::uint8_t irq ) noexcept -> Lib::Types::SystemStatus;

    private:
        /**
         * @brief 绘制像素点
         * @param video_handle 要写入的地址
         * @param color 写入像素点的颜色
         * @param X 横轴指标
         * @param Y 纵轴指标
         * @param video_long 视图长度
         */
        auto display_pixel( IN Lib::Types::Ptr< Lib::Types::uint32_t > video_handle,
                            IN Lib::Types::L_Ref< Lib::IO::DisplayColor > color,
                            IN Lib::Types::L_Ref< Lib::Types::int32_t > X,
                            IN Lib::Types::L_Ref< Lib::Types::int32_t > Y,
                            IN Lib::Types::L_Ref< Lib::Types::uint32_t > video_long ) noexcept -> Lib::Types::SystemStatus;
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
            IN Lib::Types::L_Ref< Lib::Types::uint32_t > XResolution,
            IN Lib::Types::L_Ref< Lib::Types::uint32_t > YResolution,
            IN Lib::Types::int32_t XPosition, IN Lib::Types::int32_t YPosition,
            IN Lib::Types::int32_t XCharSize, IN Lib::Types::int32_t YCharSize,
            IN Lib::Types::int64_t column, IN Lib::Types::uint64_t row,
            IN Lib::Types::Ptr< Lib::Types::uint32_t > FB_addr,
            IN Lib::Types::uint64_t FB_length ) noexcept -> Lib::Types::SystemStatus;

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
            IN CONST Lib::Types::int32_t x0, IN CONST Lib::Types::int32_t y0,
            IN CONST Lib::Types::int32_t x1, IN CONST Lib::Types::int32_t y1,
            IN Lib::Types::R_Ref< Lib::IO::DisplayColor > color )
            -> Lib::Types::SystemStatus;
        ;

    private:
        Lib::Types::Ptr< Lib::Types::GraphicsConfig > config { };
    };
}