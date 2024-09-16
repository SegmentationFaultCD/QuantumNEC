#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Hpet
    {
    public:
        struct HpetTimer
        {
            Lib::Types::uint64_t configuration_and_capability;     // 256 ~ 263
            Lib::Types::uint64_t comparator_value;                 // 264 ~ 271
            Lib::Types::uint64_t fsb_interrupt_route;              // 272 ~ 279
            Lib::Types::uint64_t reserved;                         // 280 ~ 288
        } _packed;
        struct HpetInfo
        {
            Lib::Types::uint64_t general_capabilities;        // 0 ~ 7
            Lib::Types::uint64_t reserved0;                   // 8 ~ 15
            Lib::Types::uint64_t general_configuration;       // 16 ~ 23
            Lib::Types::uint64_t reserved1;                   // 24 ~ 31
            Lib::Types::uint64_t general_intrrupt_status;     // 32 ~ 39
            Lib::Types::uint8_t reserved3[ 200 ];             // 40 ~ 239
            Lib::Types::uint64_t main_counter_value;          // 240 ~ 247
            Lib::Types::uint64_t reserved4;                   // 248 ~ 255
            HpetTimer timers[ 0 ];
        } _packed;

    public:
        /**
         * @brief 初始化 HEPT
         */
        explicit Hpet( VOID ) noexcept;

        virtual ~Hpet( VOID ) noexcept;
    };
}