#pragma once
#include <cstdint>
namespace Interrupt {

class [[gnu::packed]] Hpet {
    struct [[gnu::packed]] HpetTimer {
        std::uint64_t configuration_and_capability;
        std::uint64_t comparator_value;
        std::uint64_t fsb_interrupt_route;
        std::uint64_t reserved;
    };

private:
    struct {
        std::uint64_t REV_ID : 8;
        std::uint64_t number_of_timers : 5;
        std::uint64_t counter_size : 1;
        std::uint64_t : 1;
        std::uint64_t legacy_replacement_route_capable : 1;
        std::uint64_t vendor_id : 16;
        std::uint64_t main_counter_tick_period : 32;
    } general_capabilities;
    std::uint64_t reserved0;
    struct {
        /*
         * Overall Enable: This bit must be set to enable any of the timers to generate interrupts. If
         * this bit is 0, then the main counter will halt (will not increment) and no interrupts will be
         * caused by any of these timers.
         * • 0 – Halt main count and disable all timer interrupts
         * • 1 – allow main counter to run, and allow timer interrupts if enabled
         */
        std::uint64_t ENABLE_CNF : 1;
        /*
         * LegacyReplacement Route:
         * • 0 – Doesn’t support LegacyReplacement Route
         * • 1 – Supports LegacyReplacement Route
         * If the ENABLE_CNF bit and the LEG_RT_CNF bit are both set, then the interrupts will be
         * routed as follows:
         * Timer 0 will be routed to IRQ0 in Non-APIC or IRQ2 in the I/O APIC
         * Timer 1 will be routed to IRQ8 in Non-APIC or IRQ8 in the I/O APIC
         * Timer 2-n will be routed as per the routing in the timer n config registers.
         * If the LegacyReplacement Route bit is set, the individual routing bits for timers 0 and 1
         * (APIC or FSB) will have no impact.
         */
        std::uint64_t LEG_RT_CNF : 1;

        std::uint64_t : 62;
    } general_configuration;
    std::uint64_t reserved1;
    std::uint64_t general_intrrupt_status;
    std::uint8_t reserved3[ 200 ];
    std::uint64_t main_counter_value;
    std::uint64_t reserved4;
    HpetTimer timers[ 0 ];

public:
    explicit Hpet( void ) noexcept;
    ~Hpet( void ) = default;

public:
    auto elapsed( void ) -> std::uint64_t;
    auto nano_time( void ) -> std::uint64_t;
    auto sleep( std::uint64_t ns ) -> void;
};

inline Hpet *hpet;
}     // namespace Interrupt