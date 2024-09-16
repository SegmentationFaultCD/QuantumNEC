#include <Kernel/memory/memory.hpp>

using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;

PUBLIC namespace QuantumNEC::Kernel {
    Memory::Memory( VOID ) noexcept {
        STATIC byte_t buffer[ sizeof( Page ) + sizeof( PagingMap ) + sizeof( Heap ) ];
        byte_t *ptr { buffer };
        this->page = new ( ptr ) Page { };
        ptr += sizeof( Page );
        this->memory_paging_map = new ( ptr ) PagingMap { };
        ptr += sizeof( PagingMap );
        this->heap = new ( ptr ) Heap { };
        ptr += sizeof( Heap );
    }
}