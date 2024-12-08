#pragma once
#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <kernel/memory/page/page_manager.hpp>

#if defined( __x86_64__ )
#include <kernel/memory/x86_64/x86_64memory.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {

class Memory :
#if defined( __x86_64__ )
    public x86_64::Memory,
#elif defined( __aarch64__ )
#endif
    public PageManager,
    public KHeapManager {
public:
    explicit Memory( void ) noexcept :
        x86_64::Memory { }, PageManager { }, KHeapManager { } {
    }

    virtual ~Memory( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel