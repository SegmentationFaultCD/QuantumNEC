#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class PCSpeaker     // 蜂鸣器管理
{
public:
    explicit PCSpeaker( void ) noexcept;
    virtual ~PCSpeaker( void ) noexcept;

private:
    static auto enable( void ) -> void;
    static auto disable( void ) -> void;

public:
    static auto beep( void ) -> void;
};
}     // namespace QuantumNEC::Kernel::x86_64