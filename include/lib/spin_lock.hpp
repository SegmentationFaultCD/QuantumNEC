#pragma once
#include <atomic>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Lib {
    PUBLIC class SpinLock : std::atomic_flag
    {
    public:
        explicit SpinLock( VOID ) noexcept {
        }
        explicit SpinLock( IN BOOL locked ) noexcept {
            this->_M_i = locked;
        }
        virtual ~SpinLock( VOID ) noexcept = default;

    public:
        /**
         * @brief 释放锁
         */
        auto release( VOID ) -> VOID;
        /**
         * @brief 获取锁
         */
        auto acquire( VOID ) -> VOID;
        bool locked( ) {
            return this->_M_i;
        }
    };
}     // namespace QuantumNEC::Lib::Base
