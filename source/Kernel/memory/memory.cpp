#include <Kernel/memory/memory.hpp>

using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;

PUBLIC namespace QuantumNEC::Kernel {
    Memory::Memory( VOID ) noexcept {
        STATIC byte_t buffer[ sizeof( PageAllocater ) + sizeof( PagingMap ) + sizeof( Heap ) ];
        auto ptr { buffer };
        this->page = new ( ptr ) PageAllocater { };
        ptr += sizeof( PageAllocater );
        this->memory_paging_map = new ( ptr ) PagingMap { };
        ptr += sizeof( PagingMap );
        this->heap = new ( ptr ) Heap { };
        ptr += sizeof( Heap );
    }
}