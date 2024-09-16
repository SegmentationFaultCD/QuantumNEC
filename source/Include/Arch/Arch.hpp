#pragma once
#include <Arch/ArchInfo.hpp>
#if SYSTEM_ARCH == x86_64
#include <Arch/x86_64/platform/platform.hpp>
#elif SYSTEM_ARCH == risc_v
#include <Arch/risc-v/platform/platform.hpp>
#elif SYSTEM_ARCH == arm64
#include <Arch/aarch64/platform/platform.hpp>
#endif
#include <Arch/ArchService.hpp>
#include <concepts>

PUBLIC namespace QuantumNEC::Architecture {
    template < typename Arch >
        requires requires { typename Arch; }
                 && std::derived_from< Arch, typename Architecture::Device >
                 && std::derived_from< Arch, typename Architecture::CPUs >
                 && std::derived_from< Arch, typename Architecture::Interrupt >
                 && std::derived_from< Arch, typename Architecture::Syscall >
                 && std::derived_from< Arch, typename Architecture::SymmetricMultiprocessing >
    class ArchitectureManager : public Arch
    {
    public:
        explicit ArchitectureManager( VOID ) noexcept {
        }

    public:
        virtual ~ArchitectureManager( VOID ) noexcept = default;

    private:
    };
}
