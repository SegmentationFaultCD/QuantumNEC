#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class CMOS
    {
    private:
        enum class TimeType : Lib::Types::uint8_t {
            SECOND = 0x00,
            MINUTE = 0x02,
            HOUR = 0x04,
            WEEKDAY = 0x06,
            DAY = 0x07,
            MONTH = 0x08,
            YEAR = 0x09,
            CENTURY = 0x32,
            NMI = 0x80,
        };
        enum class TimeData {
            MINUTE = 60,            // 每分钟的秒数
            HOUR = 60 * MINUTE,     // 每小时的秒数
            DAY = 24 * HOUR,        // 每天的秒数
            YEAR = 365 * DAY,       // 每年的秒数，以 365 天算
        };
        enum class CMOSType {
            CMOS_A = 0x0a,
            CMOS_B = 0x0b,
            CMOS_C = 0x0c,
            CMOS_D = 0x0d,
        };

    public:
        class Time
        {
            friend CMOS;

        private:
            Lib::Types::int32_t sec;       // 秒数
            Lib::Types::int32_t min;       // 分钟数
            Lib::Types::int32_t hour;      // 小时数
            Lib::Types::int32_t mday;      // 一个月中天数
            Lib::Types::int32_t mon;       // 一年中月份
            Lib::Types::int32_t year;      // 从1900年开始的年数
            Lib::Types::int32_t wday;      // 一星期中某天
            Lib::Types::int32_t yday;      // 一年中某天
            Lib::Types::int32_t isdst;     // 夏令时标志
        public:
            explicit Time( VOID ) noexcept {
            }
            virtual ~Time( VOID ) noexcept {
            }

        public:
            STATIC auto time_read_bcd( IN Lib::Types::L_Ref< Time > time ) -> VOID;
            STATIC auto time_read( IN Lib::Types::L_Ref< Time > time ) -> VOID;
            STATIC auto make_time( IN Lib::Types::L_Ref< Time > time ) noexcept -> Lib::Types::time_t;
            auto time_read_bcd( VOID ) -> VOID {
                this->time_read_bcd( *this );
            }
            auto time_read( VOID ) -> VOID {
                this->time_read( *this );
            }
            auto make_time( VOID ) noexcept -> Lib::Types::time_t {
                return this->make_time( *this );
            }
        };

    public:
        explicit CMOS( VOID ) noexcept;
        virtual ~CMOS( VOID ) noexcept;

    public:
        // 读CMOS寄存器的值
        STATIC auto read_cmos( IN Lib::Types::uint8_t address ) -> Lib::Types::uint8_t;
        // 写CMOS寄存器的值
        STATIC auto write_cmos( IN Lib::Types::uint8_t address, IN Lib::Types::uint8_t value ) -> VOID;

    private:
        // 每个月开始时的已经过去天数
        inline STATIC Lib::Types::int32_t month[ 13 ] {
            0,     // 这里占位，没有 0 月，从 1 月开始
            0,
            ( 31 ),
            ( 31 + 29 ),
            ( 31 + 29 + 31 ),
            ( 31 + 29 + 31 + 30 ),
            ( 31 + 29 + 31 + 30 + 31 ),
            ( 31 + 29 + 31 + 30 + 31 + 30 ),
            ( 31 + 29 + 31 + 30 + 31 + 30 + 31 ),
            ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 ),
            ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 ),
            ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 ),
            ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 )
        };
        inline STATIC Lib::Types::time_t startup_time { };     // 一开始的时间
        inline STATIC Lib::Types::int32_t century { };         // 当前世纪值
    private:
        inline STATIC auto get_yday = []( IN Lib::Types::L_Ref< Time > time ) STATIC {
            Lib::Types::int32_t res { month[ time.mon ] };     // 已经过去的月的天数
            res += time.mday;                                  // 这个月过去的天数
            Lib::Types::int32_t year { };
            if ( time.year > 69 )
                year = time.year - 70;
            else
                year = time.year - 70 + 100;
            // 如果不是闰年，并且 2 月已经过去了，则减去一天
            // 注：1972 年是闰年，这样算不太精确，忽略了 100 年的平年
            if ( ( year + 2 ) % 4 && time.mon > 2 ) {
                res -= 1;
            }
            return res;
        };
    };
}