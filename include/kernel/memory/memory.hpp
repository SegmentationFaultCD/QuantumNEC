#pragma once
#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <kernel/memory/page/page_manager.hpp>

#if defined( __x86_64__ )
#include <kernel/memory/x86_64/x86_64memory.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {

class Memory :
    public PageManager,
    public KHeapManager,
#if defined( __x86_64__ )
    public x86_64::Memory
#elif defined( __aarch64__ )
#endif
{
public:
    explicit Memory( void ) noexcept :
        PageManager { },
        KHeapManager { },
        x86_64::Memory { } {
    }

    virtual ~Memory( void ) noexcept = default;
};     // namespace QuantumNEC::Kernel
}     // namespace QuantumNEC::Kernel