#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/memory/arch/x86_64/paging/paging.hpp>
#include <kernel/memory/arch/x86_64/paging/ptv.hpp>
#include <kernel/memory/arch/x86_64/segment/gdt.hpp>
#elif defined( __aarch64__ )
#include <kernel/memory/arch/aarch64/paging/page_table_walker.hpp>
#include <kernel/memory/arch/aarch64/paging/paging.hpp>
#include <kernel/memory/arch/aarch64/paging/ptv.hpp>
#endif

PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class MemoryArch :
        public GlobalSegmentDescriptorTable,
        public Paging {
    public:
        explicit MemoryArch( VOID ) noexcept = default;
    };
}
