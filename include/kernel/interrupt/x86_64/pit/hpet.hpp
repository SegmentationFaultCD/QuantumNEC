#pragma once
#include <lib/Uefi.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class Hpet {
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

private:
    inline static uint64_t periods;

    inline static HpetInfo *hpet_info;
};
}     // namespace QuantumNEC::Kernel::x86_64