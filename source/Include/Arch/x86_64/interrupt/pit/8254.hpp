#pragma once
#include <Lib/Types/Uefi.hpp>

PUBLIC namespace QuantumNEC::Architecture {
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