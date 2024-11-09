#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/driver/display/arch/x86_64/serial_port/serial_port.hpp>
#elif defined( __aarch64__ )
#endif
PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class DisplayArch :
#if defined( __x86_64__ )
        public SerialPort
#elif defined( __aarch64__ )
#endif
    {
    public:
        explicit DisplayArch( VOID ) noexcept = default;
    };
}