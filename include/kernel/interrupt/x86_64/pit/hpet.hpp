#pragma once
#include <lib/Uefi.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class Hpet {
private:
    enum {
        GCAP_ID   = 0x00,
        GEN_CONF  = 0x10,
        GINTR_STA = 0x20,
        MAIN_CNT  = 0xf0,
        TIM0_CONF = 0x100,
        TIM0_COMP = 0x108,
        TIM1_CONF = 0x120,
        TIM1_COMP = 0x128,
        TIM2_CONF = 0x140,
        TIM2_COMP = 0x148,
        TIM3_CONF = 0x160,
        TIM3_COMP = 0x168,
        TIM4_CONF = 0x180,
        TIM4_COMP = 0x188,
        TIM5_CONF = 0x1a0,
        TIM5_COMP = 0x1a8,
        TIM6_CONF = 0x1c0,
        TIM6_COMP = 0x1c8,
        TIM7_CONF = 0x1e0,
        TIM7_COMP = 0x1e8,
    };

public:
    struct _packed HpetTimer {
        uint64_t configuration_and_capability;
        uint64_t comparator_value;
        uint64_t fsb_interrupt_route;
        uint64_t reserved;
        explicit HpetTimer( void ) noexcept = default;
    };
    struct _packed HpetInfo {
        uint64_t  general_capabilities;
        uint64_t  reserved0;
        uint64_t  general_configuration;
        uint64_t  reserved1;
        uint64_t  general_intrrupt_status;
        uint8_t   reserved3[ 200 ];
        uint64_t  main_counter_value;
        uint64_t  reserved4;
        HpetTimer timers[ 0 ];
        explicit HpetInfo( void ) noexcept = default;
    };

public:
    /**
     * @brief 初始化 HEPT
     */
    explicit Hpet( void ) noexcept;

    virtual ~Hpet( void ) noexcept;

public:
    static auto sleep( uint64_t ns ) -> void;
    static auto enable_hpet( void ) -> void;
    static auto disable_hpet( void ) -> void;

private:
    inline static uint64_t  periods;
    inline static uint64_t  the_number_of_timer_cap;
    inline static HpetInfo *hpet_info;
};
}     // namespace QuantumNEC::Kernel::x86_64