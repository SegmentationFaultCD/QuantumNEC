#include "Kernel/Memory/map.hpp"
#include <Kernel/memory.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;
auto operator new( IN unsigned long int, IN VOID *addr ) noexcept -> VOID * {
    return addr;
}
auto operator new( IN unsigned long int size ) -> VOID * {
    return physical_to_virtual( Memory::heap->malloc( size ) );
}
auto operator new[]( IN unsigned long int size ) -> VOID * {
    return physical_to_virtual( Memory::heap->malloc( size ) );
}
auto operator delete( IN VOID *, IN unsigned long int ) noexcept -> VOID {
}
auto operator delete( IN VOID *address ) noexcept -> VOID {
    Memory::heap->free( virtual_to_physical( address ) );
}
auto operator delete[]( IN VOID *address ) noexcept -> VOID {
    Memory::heap->free( virtual_to_physical( address ) );
}
auto operator delete[]( IN VOID *, IN unsigned long int ) noexcept -> VOID {
}
PUBLIC namespace QuantumNEC::Kernel {
    PRIVATE Lib::Types::byte_t buffer[ sizeof( PageMemory ) + sizeof( MemoryMap ) + sizeof( HeapMemory ) ];
    Memory::Memory( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept {
        Lib::Types::byte_t *ptr { buffer };
        this->page = new ( ptr ) PageMemory { _config };
        ptr += sizeof( PageMemory );
        this->memory_paging = new ( ptr ) MemoryMap { _config };
        ptr += sizeof( MemoryMap );
        this->heap = new ( ptr ) HeapMemory { _config };
        ptr += sizeof( HeapMemory );
    }
}