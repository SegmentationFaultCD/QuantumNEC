#pragma once
#include <Lib/Uefi.hpp>
#include <Lib/bitmap.hpp>
#include <Kernel/print.hpp>
namespace QuantumNEC::Kernel {
PUBLIC inline constexpr CONST auto PID_COUNT { 1024ul };
class PidPool
{
public:
    explicit PidPool( VOID ) noexcept :
        bits { }, bitmap { PID_COUNT, bits } {
    }

public:
    auto allocate( VOID ) -> uint64_t {
        bitmap = { PID_COUNT, this->bits };
        auto pid_index { bitmap.allocate( 1 ) };
        bitmap[ pid_index ] = { pid_index, 1 };
        return pid_index;
    }

private:
    Lib::Bitmap::BitmapEntry bits[ PID_COUNT ] { };
    Lib::Bitmap bitmap { PID_COUNT, bits };
};
PUBLIC inline PidPool pid_pool { };
}     // namespace QuantumNEC::Kernel