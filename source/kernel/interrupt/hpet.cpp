#include <kernel/driver/cpu/mmio.hpp>
#include <kernel/interrupt/hpet.hpp>
namespace Interrupt {
auto Hpet::elapsed( void ) -> std::uint64_t {
    return ( this->main_counter_value * this->general_capabilities.main_counter_tick_period ) / 1000000U;
}
auto Hpet::nano_time( void ) -> std::uint64_t {
    return this->main_counter_value * ( this->general_capabilities.main_counter_tick_period );
}
auto Hpet::sleep( std::uint64_t ns ) -> void {
    auto targetTime = this->nano_time( );
    uint64_t after = 0;
    // copy from cpos
    while ( true ) {
        uint64_t n = nano_time( );
        if ( n < targetTime ) {
            after += 0xffffffff - targetTime + n;
            targetTime = n;
        }
        else {
            after += n - targetTime;
            targetTime = n;
        }
        if ( after >= ns ) {
            return;
        }
    }
}
Hpet::Hpet( void ) noexcept {
    this->main_counter_value = 0;
    this->general_configuration.ENABLE_CNF = 1;
    this->general_configuration.LEG_RT_CNF = 0;
    // 不要传统映射
}
}     // namespace Interrupt