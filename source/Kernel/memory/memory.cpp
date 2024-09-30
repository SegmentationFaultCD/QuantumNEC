#include <Kernel/memory/memory.hpp>

using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;

PUBLIC namespace QuantumNEC::Kernel {
    Memory::Memory( VOID ) noexcept {
        STATIC byte_t pabuf[ sizeof( PageAllocater ) ];
        STATIC byte_t pmbuf[ sizeof( PagingMap ) ];
        STATIC byte_t hbuf[ sizeof( Heap ) ];
        this->page = new ( pabuf ) PageAllocater { };
        this->memory_paging_map = new ( pmbuf ) PagingMap { };
        this->heap = new ( hbuf ) Heap { };
    }
}