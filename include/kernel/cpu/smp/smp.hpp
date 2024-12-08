#pragma once
#include <lib/Uefi.hpp>

#if defined( __x86_64__ )
#include <kernel/cpu/smp/x86_64/x86_64smp.hpp>
#elif defined( __aarch64__ )
#endif
namespace QuantumNEC::Kernel {
class SymmetricMultiprocessing :
#if defined( __x86_64__ )
    public x86_64::SymmetricMultiprocessing
#elif defined( __aarch64__ )
#endif
{
public:
    explicit SymmetricMultiprocessing( void ) noexcept :
        x86_64::SymmetricMultiprocessing { } {
    }
    virtual ~SymmetricMultiprocessing( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel
