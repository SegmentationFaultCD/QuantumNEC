#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Utils/utils.hpp>
PUBLIC namespace QuantumNEC::Lib::IO {
    PUBLIC constexpr CONST auto ZEROPAD { 1 };
    PUBLIC constexpr CONST auto SIGN { 2 };
    PUBLIC constexpr CONST auto PLUS { 4 };
    PUBLIC constexpr CONST auto SPACE { 8 };
    PUBLIC constexpr CONST auto LEFT { 16 };
    PUBLIC constexpr CONST auto SPECIAL { 32 };
    PUBLIC constexpr CONST auto SMALL { 64 };
    /**
     * @brief 显示颜色
     */
    PUBLIC enum class DisplayColor : int {
        WHITE = 0x00ffffff,          // 白
        BLACK = 0x00000000,          // 黑
        RED = 0x00ff0000,            // 红
        ORANGE = 0x00ff8000,         // 橙
        YELLOW = 0x00ffff00,         // 黄
        GREEN = 0x0000ff00,          // 绿
        BLUE = 0x000040ac,           // 蓝
        INDIGO = 0x0000ffff,         // 青
        PURPLE = 0x008000ff,         // 紫
        GRAY = 0x00a4a4a4,           // 灰
        BLUEGRAY = 0x005486ab,       // 蓝灰
        YELLOWGRAY = 0x009da682,     // 黄灰
        INDIGOGRAY = 0x00b5eaea,     // 青灰
        GRAYISH = 0x008d8d8d         // 淡灰
    };
    PUBLIC typedef struct
    {
        Lib::Types::int32_t XResolution;
        Lib::Types::int32_t YResolution;
        Lib::Types::int32_t XPosition;
        Lib::Types::int32_t YPosition;
        Lib::Types::int32_t XCharSize;
        Lib::Types::int32_t YCharSize;
        Lib::Types::Ptr< Lib::Types::uint32_t > FB_addr;
        Lib::Types::uint64_t FB_length;
        Lib::Types::int64_t column;
        Lib::Types::uint64_t row;
    } Position;
    // 64kb缓冲区
    PUBLIC Lib::Types::char_t buffer[ 0x10000 ] { };
    /**
     * @brief 基本输出
     */
    PUBLIC class BasePrint
    {
    public:
        explicit( TRUE ) BasePrint( VOID ) noexcept {
        }
        explicit( TRUE ) BasePrint( IN Lib::Types::L_Ref< CONST Position > _Pos ) noexcept;
        virtual ~BasePrint( VOID ) noexcept = default;

    public:
        /**
         * @brief 获取Pos
         */
        auto getPos( IN Lib::Types::L_Ref< CONST Position > Pos ) CONST->VOID;
        /**
         * @brief 给予Pos
         */
        auto putPos( VOID ) CONST->Lib::Types::L_Ref< CONST Position >;

    public:
        /**
         * @brief 输出日志
         * @param FRcolor 前景色
         * @param BKcolor 背景色
         * @param fmt 格式字符串
         * @param args 参数
         */
        auto printk( IN DisplayColor FRcolor, IN DisplayColor BKcolor, IN Lib::Types::Ptr< CONST Lib::Types::char_t > fmt, IN... ) -> Lib::Types::SystemStatus;

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
            IN Lib::Types::Ptr< Lib::Types::uint32_t > FB,
            IN Lib::Types::int32_t Xsize, IN Lib::Types::int32_t X,
            IN Lib::Types::int32_t Y, IN DisplayColor FRcolor,
            IN DisplayColor BKcolor, IN Lib::Types::uchar_t Font ) -> VOID;

    private:
        mutable Position Pos;
    };
}