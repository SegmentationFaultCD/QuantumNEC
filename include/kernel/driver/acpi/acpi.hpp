#pragma once
#include <lib/Uefi.hpp>

#if defined( __x86_64__ )
#include <kernel/driver/acpi/x86_64/acpi.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {
class Acpi :
#if defined( __x86_64__ )
    public x86_64::Acpi
#elif defined( __aarch64__ )
#endif
{
public:
    explicit Acpi( void ) noexcept :
        x86_64::Acpi { } {
    }
    virtual ~Acpi( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel