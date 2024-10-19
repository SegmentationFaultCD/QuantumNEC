#pragma once
#include <lib/Uefi.hpp>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Hpet
    {
    public:
        struct HpetTimer
        {
            uint64_t configuration_and_capability;     // 256 ~ 263
            uint64_t comparator_value;                 // 264 ~ 271
            uint64_t fsb_interrupt_route;              // 272 ~ 279
            uint64_t reserved;                         // 280 ~ 288
            explicit HpetTimer( VOID ) noexcept = default;
        } _packed;
        struct HpetInfo
        {
            uint64_t general_capabilities;        // 0 ~ 7
            uint64_t reserved0;                   // 8 ~ 15
            uint64_t general_configuration;       // 16 ~ 23
            uint64_t reserved1;                   // 24 ~ 31
            uint64_t general_intrrupt_status;     // 32 ~ 39
            uint8_t reserved3[ 200 ];             // 40 ~ 239
            uint64_t main_counter_value;          // 240 ~ 247
            uint64_t reserved4;                   // 248 ~ 255
            HpetTimer timers[ 0 ];
            explicit HpetInfo( VOID ) noexcept = default;
        } _packed;

    public:
        /**
         * @brief 初始化 HEPT
         */
        explicit Hpet( VOID ) noexcept;

        virtual ~Hpet( VOID ) noexcept;
    };
}