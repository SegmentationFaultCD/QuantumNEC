#pragma once
#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/driver/output/arch/x86_64/serial_port.hpp>
#elif defined( __aarch64__ )
#endif

PUBLIC namespace QuantumNEC::Kernel {
    class OutputArch :
#if defined( __x86_64__ )
        public x86_64::SerialPort
#elif defined( __aarch64__ )
#endif
    {
    public:
        explicit OutputArch( VOID ) noexcept = default;
        virtual ~OutputArch( VOID ) noexcept = default;
    };
}