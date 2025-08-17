#pragma once
#include <concepts>
#include <cstdint>
#include <kernel/driver/cpu/register.hpp>
namespace Driver {
class IO {
public:
    struct CpuidStatus {
        enum : int {
            CPUID_FEAT_RCX_SSE3          = 1 << 0,
            CPUID_FEAT_RCX_PCLMUL        = 1 << 1,
            CPUID_FEAT_RCX_DTES64        = 1 << 2,
            CPUID_FEAT_RCX_MONITOR       = 1 << 3,
            CPUID_FEAT_RCX_DS_CPL        = 1 << 4,
            CPUID_FEAT_RCX_VMX           = 1 << 5,
            CPUID_FEAT_RCX_SMX           = 1 << 6,
            CPUID_FEAT_RCX_EST           = 1 << 7,
            CPUID_FEAT_RCX_TM2           = 1 << 8,
            CPUID_FEAT_RCX_SSSE3         = 1 << 9,
            CPUID_FEAT_RCX_CID           = 1 << 10,
            CPUID_FEAT_RCX_SDBG          = 1 << 11,
            CPUID_FEAT_RCX_FMA           = 1 << 12,
            CPUID_FEAT_RCX_CX16          = 1 << 13,
            CPUID_FEAT_RCX_XTPR          = 1 << 14,
            CPUID_FEAT_RCX_PDCM          = 1 << 15,
            CPUID_FEAT_RCX_LEVEL5_PAGING = 1 << 16,
            CPUID_FEAT_RCX_PCID          = 1 << 17,
            CPUID_FEAT_RCX_DCA           = 1 << 18,
            CPUID_FEAT_RCX_SSE4_1        = 1 << 19,
            CPUID_FEAT_RCX_SSE4_2        = 1 << 20,
            CPUID_FEAT_RCX_X2APIC        = 1 << 21,
            CPUID_FEAT_RCX_MOVBE         = 1 << 22,
            CPUID_FEAT_RCX_POPCNT        = 1 << 23,
            CPUID_FEAT_RCX_TSC           = 1 << 24,
            CPUID_FEAT_RCX_AES           = 1 << 25,
            CPUID_FEAT_RCX_XSAVE         = 1 << 26,
            CPUID_FEAT_RCX_OSXSAVE       = 1 << 27,
            CPUID_FEAT_RCX_AVX           = 1 << 28,
            CPUID_FEAT_RCX_F16C          = 1 << 29,
            CPUID_FEAT_RCX_RDRAND        = 1 << 30,
            CPUID_FEAT_RCX_HYPERVISOR    = 1 << 31,
            CPUID_FEAT_RDX_FPU           = 1 << 0,
            CPUID_FEAT_RDX_VME           = 1 << 1,
            CPUID_FEAT_RDX_DE            = 1 << 2,
            CPUID_FEAT_RDX_PSE           = 1 << 3,
            CPUID_FEAT_RDX_TSC           = 1 << 4,
            CPUID_FEAT_RDX_MSR           = 1 << 5,
            CPUID_FEAT_RDX_PAE           = 1 << 6,
            CPUID_FEAT_RDX_MCE           = 1 << 7,
            CPUID_FEAT_RDX_CX8           = 1 << 8,
            CPUID_FEAT_RDX_APIC          = 1 << 9,
            CPUID_FEAT_RDX_SEP           = 1 << 11,
            CPUID_FEAT_RDX_MTRR          = 1 << 12,
            CPUID_FEAT_RDX_PGE           = 1 << 13,
            CPUID_FEAT_RDX_MCA           = 1 << 14,
            CPUID_FEAT_RDX_CMOV          = 1 << 15,
            CPUID_FEAT_RDX_PAT           = 1 << 16,
            CPUID_FEAT_RDX_PSE36         = 1 << 17,
            CPUID_FEAT_RDX_PSN           = 1 << 18,
            CPUID_FEAT_RDX_CLFLUSH       = 1 << 19,
            CPUID_FEAT_RDX_DS            = 1 << 21,
            CPUID_FEAT_RDX_ACPI          = 1 << 22,
            CPUID_FEAT_RDX_MMX           = 1 << 23,
            CPUID_FEAT_RDX_FXSR          = 1 << 24,
            CPUID_FEAT_RDX_SSE           = 1 << 25,
            CPUID_FEAT_RDX_SSE2          = 1 << 26,
            CPUID_FEAT_RDX_SS            = 1 << 27,
            CPUID_FEAT_RDX_HTT           = 1 << 28,
            CPUID_FEAT_RDX_TM            = 1 << 29,
            CPUID_FEAT_RDX_IA64          = 1 << 30,
            CPUID_FEAT_RDX_PBE           = 1 << 31
        };
        std::uint64_t mop;
        std::uint64_t sop;
        std::uint64_t rax;
        std::uint64_t rbx;
        std::uint64_t rcx;
        std::uint64_t rdx;
        CpuidStatus( uint64_t _m, uint64_t _s, uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d ) noexcept {
            this->mop = _m;
            this->sop = _s;
            this->rax = _a;
            this->rbx = _b;
            this->rcx = _c;
            this->rdx = _d;
        }

        CpuidStatus( std::same_as< CpuidStatus > auto &&status ) noexcept {
            this->mop = status.mop;
            this->sop = status.sop;
            this->rax = status.rax;
            this->rbx = status.rbx;
            this->rcx = status.rcx;
            this->rdx = status.rdx;
        }
        CpuidStatus( void ) noexcept {
        }
    };

public:
    static auto cpuid( CpuidStatus &status ) -> CpuidStatus &;
    static auto cpuid( CpuidStatus &&status ) -> CpuidStatus;
    static auto cli( void ) -> void;
    static auto sti( void ) -> void;
    static auto hlt( void ) -> void;
    static auto nop( void ) -> void;
    static auto rdmsr( std::uint64_t address ) -> std::uint64_t;
    static auto wrmsr( std::uint64_t address, std::uint64_t value ) -> void;
    static auto get_rsp( void ) -> std::uint64_t;
    static auto get_rflags( void ) -> std::uint64_t;

    static auto in8( std::uint16_t port ) -> std::uint8_t;
    static auto in16( std::uint16_t port ) -> std::uint16_t;
    static auto in32( std::uint16_t port ) -> std::uint32_t;
    static auto out8( std::uint16_t port, std::uint8_t value ) -> void;
    static auto out16( std::uint16_t port, std::uint16_t value ) -> void;
    static auto out32( std::uint16_t port, std::uint32_t value ) -> void;

    static auto read_cr8( void ) -> ControlRegisters::CR8;
    static auto write_cr8( ControlRegisters::CR8 cr8 ) -> void;
    static auto read_cr4( void ) -> ControlRegisters::CR4;
    static auto write_cr4( ControlRegisters::CR4 cr4 ) -> void;
    static auto read_cr3( void ) -> ControlRegisters::CR3;
    static auto write_cr3( ControlRegisters::CR3 cr3 ) -> void;
    static auto read_cr2( void ) -> ControlRegisters::CR2;
    static auto write_cr2( ControlRegisters::CR2 cr2 ) -> void;
    static auto read_cr0( void ) -> ControlRegisters::CR0;
    static auto write_cr0( ControlRegisters::CR0 cr0 ) -> void;
    static auto invlpg( void *address ) -> void;
    static auto pause( void ) -> void;
    static auto mfence( void ) -> void;
    static auto lfence( void ) -> void;
    static auto sfence( void ) -> void;
    static auto set_page_table( std::uint64_t *mmap ) -> void;
    static auto get_page_table( void ) -> uint64_t *;
    static auto flush_tlb( void ) -> void;
};
}     // namespace Driver