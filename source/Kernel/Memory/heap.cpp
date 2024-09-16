#include <Kernel/memory.hpp>
#include <Lib/STL/string>
#include <Lib/Base/deflib.hpp>
#include <Arch/Arch.hpp>
#include <Lib/IO/Stream/iostream>
#include <Kernel/task.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Kernel;
    PRIVATE TaskLock lock { };
}

PUBLIC namespace QuantumNEC::Kernel {
    HeapMemory::HeapMemory( IN Lib::Types::Ptr< Lib::Types::BootConfig > ) noexcept {
        Lib::STL::list_init( &this->global_memory_zone_table );
        Lib::Types::Ptr< Zone > zone { reinterpret_cast< decltype( zone ) >( Memory::page->malloc( 1, PageMemory::MemoryPageType::PAGE_2M ) ) };
        Lib::STL::list_init( &zone->block_table );
        zone->zone_node.container = reinterpret_cast< decltype( zone->zone_node.container ) >( zone );
        zone->is_full = FALSE;
        zone->zone_start_address = reinterpret_cast< decltype( zone->zone_start_address ) >( reinterpret_cast< Lib::Types::uint64_t >( zone ) + sizeof *zone );
        zone->zone_end_address = reinterpret_cast< decltype( zone->zone_end_address ) >( reinterpret_cast< Lib::Types::uint64_t >( zone ) + PAGE_SIZE );
        zone->size = PAGE_SIZE;
        Lib::STL::list_add_to_end( &this->global_memory_zone_table, &zone->zone_node );
    }
    auto HeapMemory::malloc( IN Lib::Types::size_t size )->Lib::Types::Ptr< VOID > {
        lock.acquire( );
        if ( size <= 0 ) {
            lock.release( );
            return NULL;
        }
        Lib::Types::Ptr< Zone > zone { };
        Lib::Types::Ptr< Block > block { };
        if ( size < PAGE_SIZE ) {
            // 不超过一页大小的就在页内分配
            zone = reinterpret_cast< decltype( zone ) >( Lib::STL::list_traversal(
                                                             &global_memory_zone_table,
                                                             [ & ]( Lib::Types::Ptr< Lib::STL::ListNode > node, Lib::Types::uint32_t ) -> Lib::Types::BOOL { return !reinterpret_cast< decltype( zone ) >( node->container )->is_full; }, 0 )
                                                             ->container );

            if ( Lib::STL::list_is_empty( &zone->block_table ) ) {
                // 如果整个块都为空闲状态，那么就直接在zone结构后面作为起始地址开始分配
                // 也就是从这个内存区域开头开始分配
                block = reinterpret_cast< decltype( block ) >( zone->zone_start_address );
            }
            else {
                // 如果这块内存区域不为空，那么就查找是否有之前分配的符合条件的内存块
                Lib::Types::Ptr< Lib::STL::ListNode > node { Lib::STL::list_traversal(
                    &zone->block_table, [ & ]( Lib::Types::Ptr< Lib::STL::ListNode > node, Lib::Types::uint32_t ) -> Lib::Types::BOOL { return reinterpret_cast< decltype( block ) >( node->container )->is_free && ( reinterpret_cast< decltype( block ) >( node->container )->size == size ); }, 0 ) };
                if ( !node ) {
                    // 如果这个块中没有以前分配的符合条件的内存块，那么就开辟新的内存块

                    block = reinterpret_cast< decltype( block ) >( reinterpret_cast< Lib::Types::uint64_t >( reinterpret_cast< decltype( block ) >( zone->block_table.end.prev->container )->end_address ) );
                    if ( reinterpret_cast< Lib::Types::uint64_t >( block ) + size > reinterpret_cast< Lib::Types::uint64_t >( zone->zone_end_address ) ) {
                        // 如果剩余内存区域空间不足以分配一个新内存块
                        if ( auto last_free_memory { reinterpret_cast< Lib::Types::uint64_t >( zone->zone_end_address ) - reinterpret_cast< Lib::Types::uint64_t >( block ) }; last_free_memory > sizeof *block ) {
                            // 如果剩下的空间支持分配1字节及以上的内存+block块大小那么就将它整理
                            block->owner = zone;
                            block->block_node.container = reinterpret_cast< decltype( block->block_node.container ) >( block );
                            Lib::STL::list_add_to_end( &zone->block_table, &block->block_node );
                            block->start_address = reinterpret_cast< decltype( block->start_address ) >( reinterpret_cast< Lib::Types::uint64_t >( block ) + sizeof *block );
                            block->end_address = reinterpret_cast< decltype( block->end_address ) >( reinterpret_cast< Lib::Types::uint64_t >( block->start_address ) + last_free_memory );
                            block->size = last_free_memory;
                            block->is_free = FALSE;
                            block->block_magic = MEMORY_BLOCK_MAGIC_NUMBER;
                        }
                        // 如果连block都不足已分配那么直接放弃这块内存，设置整个内存块已满
                        zone->is_full = TRUE;
                        // 并开辟新内存区域
                        zone = reinterpret_cast< decltype( zone ) >( ( Memory::page->malloc( 1, PageMemory::MemoryPageType::PAGE_2M ) ) );
                        Lib::STL::list_init( &zone->block_table );
                        zone->zone_node.container = reinterpret_cast< decltype( zone->zone_node.container ) >( zone );
                        zone->is_full = FALSE;
                        zone->size = PAGE_SIZE;
                        zone->zone_start_address = reinterpret_cast< decltype( zone->zone_start_address ) >( reinterpret_cast< Lib::Types::uint64_t >( zone ) + sizeof *zone );
                        zone->zone_end_address = reinterpret_cast< decltype( zone->zone_end_address ) >( reinterpret_cast< Lib::Types::uint64_t >( zone ) + zone->size );
                        Lib::STL::list_add_to_end( &global_memory_zone_table, &zone->zone_node );
                        block = reinterpret_cast< decltype( block ) >( zone->zone_start_address );
                    }
                }
                else {
                    block = reinterpret_cast< decltype( block ) >( node->container );
                    // 有，那么就设置标志位为FALSE并返回这个内存块指向的地址
                    block->is_free = FALSE;
                    zone->block_count++;
                    Lib::STL::memset( block->start_address, 0, size );
                    lock.release( );
                    return block->start_address;
                }
            }
        }
        else if ( size >= PAGE_SIZE ) {
            Lib::Types::uint64_t page_count { size % PAGE_SIZE ? size / PAGE_SIZE + 1 : size / PAGE_SIZE };
            // 超过一页大小的分配量那么直接分配多个页或者一个整页
            zone = reinterpret_cast< decltype( zone ) >( reinterpret_cast< Lib::Types::uint64_t >( Memory::page->malloc( page_count, PageMemory::MemoryPageType::PAGE_2M ) ) );
            zone->zone_node.container = reinterpret_cast< decltype( zone->zone_node.container ) >( zone );
            zone->is_full = size % PAGE_SIZE ? FALSE : TRUE;
            zone->size = page_count * PAGE_SIZE;
            zone->zone_start_address = reinterpret_cast< decltype( zone->zone_start_address ) >( reinterpret_cast< Lib::Types::uint64_t >( zone ) + sizeof *zone );
            zone->zone_end_address = reinterpret_cast< decltype( zone->zone_end_address ) >( reinterpret_cast< Lib::Types::uint64_t >( zone ) + zone->size );
            Lib::STL::list_add_to_end( &global_memory_zone_table, &zone->zone_node );
            block = reinterpret_cast< decltype( block ) >( zone->zone_start_address );
        }
        // 初始化内存块
        block->block_node.container = reinterpret_cast< decltype( block->block_node.container ) >( block );
        Lib::STL::list_add_to_end( &zone->block_table, &block->block_node );
        block->owner = zone;
        block->start_address = reinterpret_cast< decltype( block->start_address ) >( reinterpret_cast< Lib::Types::uint64_t >( block ) + sizeof *block );
        block->end_address = reinterpret_cast< decltype( block->end_address ) >( reinterpret_cast< Lib::Types::uint64_t >( block->start_address ) + size );
        block->size = size;
        block->is_free = FALSE;
        block->block_magic = MEMORY_BLOCK_MAGIC_NUMBER;
        zone->block_count++;
        Lib::STL::memset( block->start_address, 0, size );
        lock.release( );
        return block->start_address;
    }
    auto HeapMemory::free( IN Lib::Types::Ptr< VOID > address )->VOID {
        lock.acquire( );
        if ( !address ) {
            lock.release( );
            return;
        }
        // 获取block和zone
        Lib::Types::Ptr< Block > block { reinterpret_cast< decltype( block ) >( reinterpret_cast< Lib::Types::uint64_t >( address ) - sizeof *block ) };
        Lib::Types::Ptr< Zone > zone { block->owner };
        // 检测是否为正规的地址
        if ( block->block_magic != MEMORY_BLOCK_MAGIC_NUMBER ) {
            lock.release( );
            return;
        }
        // 清空这块内存并设置为空闲
        Lib::STL::memset( block->start_address, 0, block->size );
        block->is_free = TRUE;
        zone->block_count--;
        if ( !zone->block_count && Lib::STL::list_length( &global_memory_zone_table ) > 1 ) {
            // 如果整个内存块空闲值为0，那么直接清除整个内存块并从链表中剔除
            // 但是前提是它不是最后一个块
            // 如果是最后一个块那么也是啥也不干，因为要保证一定有一块内存
            list_delete( &zone->zone_node );
            Memory::page->free( zone, zone->size / PAGE_SIZE, PageMemory::MemoryPageType::PAGE_2M );
            Lib::STL::memset( zone, 0, zone->size );
        }
        // 反之，啥也不干
        lock.release( );
        return;
    }
}
