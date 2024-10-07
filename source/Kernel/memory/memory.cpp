#include <Kernel/memory/memory.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    Memory::Memory( VOID ) noexcept :
        __page_manager { },
        __paging { },
        __heap_manager { } {
    }
}