#pragma once
#include <kernel/interrupt/x86_64/pic/apic.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class ApicTimer {
private:
    static constexpr auto INTERVAL { 1 };
    static constexpr auto DIVISOR { 3 };

public:
    explicit ApicTimer( void ) noexcept;

public:
    static auto enable( void ) -> void;
    static auto disable( void ) -> void;
    static auto install( uint64_t tick = ApicTimer::ticks_result ) -> void;
    static auto uninstall( void ) -> void;
    static auto get_current_count( void ) -> uint64_t;
    static auto set_current_count( uint64_t ) -> void;

public:
    inline static uint64_t ticks_result;
    inline static char     measure_apic_timer_flag;
};

}     // namespace QuantumNEC::Kernel::x86_64