#include <Kernel/memory/manager/heap/heap_manager.hpp>
#include <Kernel/memory/allocater/page/page_allocater.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    __heap_manager::__heap_manager( VOID ) noexcept {
        this->global_memory_zone_table.init( );

        auto zone = new ( physical_to_virtual( __page_allocater { }.__allocate< MemoryPageType::PAGE_2M >( 1 ) ) ) __zone { };

        zone->zone_node.container = reinterpret_cast< decltype( zone->zone_node.container ) >( zone );
        zone->is_full = FALSE;
        zone->zone_start_address = reinterpret_cast< decltype( zone->zone_start_address ) >( reinterpret_cast< uint64_t >( zone ) + sizeof *zone );
        zone->zone_end_address = reinterpret_cast< decltype( zone->zone_end_address ) >( reinterpret_cast< uint64_t >( zone ) + __page_allocater::__page_size< MemoryPageType::PAGE_2M > );
        zone->size = __page_allocater::__page_size< MemoryPageType::PAGE_2M >;

        this->global_memory_zone_table.append( zone->zone_node );
    }
}