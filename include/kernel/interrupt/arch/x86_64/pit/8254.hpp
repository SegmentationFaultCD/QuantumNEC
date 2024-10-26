#pragma once
#include <lib/Uefi.hpp>

PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class P8254
    {
    public:
        /**
         * @brief 初始化 8254 pit
         */
        explicit P8254( VOID ) noexcept;

        virtual ~P8254( VOID ) noexcept;
    };
}