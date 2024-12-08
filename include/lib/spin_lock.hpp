#pragma once
#include <atomic>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Lib {
class SpinLock : std::atomic_flag {
public:
    explicit SpinLock( void ) noexcept {
    }
    explicit SpinLock( IN bool locked ) noexcept {
        this->_M_i = locked;
    }
    virtual ~SpinLock( void ) noexcept = default;

public:
    /**
     * @brief 释放锁
     */
    auto release( void ) -> void;
    /**
     * @brief 获取锁
     */
    auto acquire( void ) -> void;
    bool locked( ) {
        return this->_M_i;
    }
};
}     // namespace QuantumNEC::Lib
