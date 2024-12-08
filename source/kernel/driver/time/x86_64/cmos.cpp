#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/time/x86_64/cmos.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
using namespace Lib;

CMOS::CMOS( void ) noexcept {
    Time time { };
    // 读取当前时间
    time.time_read( );
    this->startup_time = time.make_time( );
    println< print_level::SYSTEM >( "Startup time : {}{}-{}-{} {}:{}:{} ", century, time.year, time.mon, time.mday, time.hour, time.min, time.sec );
    // 开启CMOS中断
    this->write_cmos( static_cast< uint8_t >( CMOSType::CMOS_B ), 0b01000010 );
    // 读C寄存器，以允许CMOS中断
    this->read_cmos( static_cast< uint8_t >( CMOSType::CMOS_C ) );
    // 设置CMOS中断频率
    CPU::io_out8( static_cast< uint8_t >( CMOSType::CMOS_A ), ( CPU::io_in8( static_cast< uint8_t >( CMOSType::CMOS_A ) ) & 0xf ) | 14 );
}
CMOS::~CMOS( void ) noexcept {
}
auto CMOS::Time::make_time( IN Time &time ) noexcept -> time_t {
    time_t  res { };
    int32_t year { };     // 1970 年开始的年数
    // 下面从 1900 年开始的年数计算
    if ( time.year >= 70 )
        year = time.year - 70;
    else
        year = time.year - 70 + 100;
    // 这些年经过的秒数时间
    res = static_cast< time_t >( TimeData::YEAR ) * year;
    // 已经过去的闰年，每个加 1 天
    res += static_cast< time_t >( TimeData::DAY ) * ( ( year + 1 ) / 4 );
    // 已经过完的月份的时间
    res += month[ time.mon ] * static_cast< time_t >( TimeData::DAY );
    // 如果 2 月已经过了，并且当前不是闰年，那么减去一天
    if ( time.mon > 2 && ( ( year + 2 ) % 4 ) )
        res -= static_cast< time_t >( TimeData::DAY );
    // 这个月已经过去的天
    res += static_cast< time_t >( TimeData::DAY ) * ( time.mday - 1 );
    // 今天过去的小时
    res += static_cast< time_t >( TimeData::HOUR ) * time.hour;
    // 这个小时过去的分钟
    res += static_cast< time_t >( TimeData::MINUTE ) * time.min;
    // 这个分钟过去的秒
    res += time.sec;

    return res;
}

auto CMOS::read_cmos( IN uint8_t address ) -> uint8_t {
    CPU::io_out8( CMOS_ADDRESS, static_cast< uint8_t >( TimeType::NMI ) | static_cast< uint8_t >( address ) );
    auto value = CPU::io_in8( CMOS_DATA );

    return value;
}
auto CMOS::write_cmos( IN uint8_t address, IN uint8_t value ) -> void {
    CPU::io_out8( CMOS_ADDRESS, static_cast< uint8_t >( TimeType::NMI ) | static_cast< uint8_t >( address ) );
    CPU::io_out8( CMOS_DATA, value );
}
auto CMOS::Time::time_read( IN Time &time ) -> void {
    time_read_bcd( time );
    time.sec   = BCD_TO_BIN( time.sec );
    time.min   = BCD_TO_BIN( time.min );
    time.hour  = BCD_TO_BIN( time.hour );
    time.wday  = BCD_TO_BIN( time.wday );
    time.mday  = BCD_TO_BIN( time.mday );
    time.mon   = BCD_TO_BIN( time.mon );
    time.year  = BCD_TO_BIN( time.year );
    time.yday  = get_yday( time );
    time.isdst = -1;
    century    = BCD_TO_BIN( century );
}
auto CMOS::Time::time_read_bcd( IN Time &time ) -> void {
    // CMOS 的访问速度很慢。为了减小时间误差，在读取了下面循环中所有数值后，
    // 若此时 CMOS 中秒值发生了变化，那么就重新读取所有值。
    // 这样内核就能把与 CMOS 的时间误差控制在 1 秒之内。
    do {
        time.sec  = CMOS::read_cmos( static_cast< uint8_t >( TimeType::SECOND ) );
        time.min  = CMOS::read_cmos( static_cast< uint8_t >( TimeType::MINUTE ) );
        time.hour = CMOS::read_cmos( static_cast< uint8_t >( TimeType::HOUR ) );
        time.wday = CMOS::read_cmos( static_cast< uint8_t >( TimeType::WEEKDAY ) );
        time.mday = CMOS::read_cmos( static_cast< uint8_t >( TimeType::DAY ) );
        time.mon  = CMOS::read_cmos( static_cast< uint8_t >( TimeType::MONTH ) );
        time.year = CMOS::read_cmos( static_cast< uint8_t >( TimeType::YEAR ) );
        century   = CMOS::read_cmos( static_cast< uint8_t >( TimeType::CENTURY ) );
    } while ( time.sec != CMOS::read_cmos( static_cast< uint8_t >( TimeType::SECOND ) ) );
}
}     // namespace QuantumNEC::Kernel::x86_64
