#pragma once

#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/driver/time/x86_64/cmos.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {
class Time :
#if defined( __x86_64__ )
    public x86_64::CMOS
#elif defined( __aarch64__ )
#endif
{
public:
    explicit Time( void ) noexcept = default;
    virtual ~Time( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel