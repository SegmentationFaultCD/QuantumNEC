#pragma once

#include <kernel/driver/output/screen/screen.hpp>

#include <lib/Uefi.hpp>

#if defined( __x86_64__ )
#include <kernel/driver/output/x86_64/serial_port.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {
class Output :
    public Screen,
#if defined( __x86_64__ )
    public x86_64::SerialPort
#elif defined( __aarch64__ )
#endif
{
public:
    explicit Output( void ) noexcept = default;
    virtual ~Output( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel