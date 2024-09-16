#include <Kernel/memory/memory_page_map/memory_map_4K.hpp>
#include <Kernel/memory/pmlxt/pmlxt.hpp>
#include <Libcxx/string.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC::Lib;
    PRIVATE SpinLock lock { };
}
PUBLIC namespace QuantumNEC::Kernel {
    auto MemoryPageMap4K::malloc( IN uint64_t size )->VOID * {
        lock.acquire( );
        if ( !size ) {
            lock.release( );
            // size空的就没有分配
            return NULL;
        }
        PageHandler *handler { };
        MemoryPageMap2M mpm2m { };
        auto allocate_information_block = [ & ] {
            auto _handler = new ( physical_to_virtual( block_info_address_start ) ) PageHandler { };
            block_info_address_start += sizeof *_handler;
            _handler->node.container = _handler;
            memory_page_table_4k.push( _handler->node );                   // 将节点插入全局队列
            _handler->map_base_adderess = (uint64_t)mpm2m.malloc( 1 );     // 确定base地址，用于free时借助算法节约时间
            return _handler;
        };
        if ( size >= MEMORY_PAGE_DESCRIPTOR ) {     // 这直接分配一块 PAGE_1G_SIZE * MEMORY_PAGE_DESCRIPTOR内存了
            // 首先计算handler的数量，如果handler的数量恰好可以除得尽那么就是size / MEMORY_PAGE_DESCRIPTOR，如果除不尽那就向上取整
            auto handler_count = ( size % MEMORY_PAGE_DESCRIPTOR ) ? size / MEMORY_PAGE_DESCRIPTOR : DIV_ROUND_UP( size, MEMORY_PAGE_DESCRIPTOR );
            auto flag = ( size % MEMORY_PAGE_DESCRIPTOR ) == 0;
            if ( !flag ) {     // 除的尽
                // 全部填充
                while ( --handler_count ) {
                    handler = allocate_information_block( );
                    for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
                        handler->bitmap[ i ] = { i, BITMAP_MASK };
                    }
                    handler->flags.set_full( PAGE_FLAGS_FULL );
                    memset( physical_to_virtual( handler->map_base_adderess ), 0, PAGE_4K_SIZE * MEMORY_PAGE_DESCRIPTOR );
                }
            }
            else {
                // 头块，记录
                auto _handler = allocate_information_block( );
                handler = _handler;
                for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
                    _handler->bitmap[ i ] = { i, BITMAP_MASK };
                }
                _handler->flags.set_full( PAGE_FLAGS_FULL );
                memset( physical_to_virtual( _handler->map_base_adderess ), 0, PAGE_4K_SIZE * MEMORY_PAGE_DESCRIPTOR );
                // 头块后面的所有块，但不包括最后一块
                for ( auto c { 1ul }; c < handler_count - 1; ++c ) {
                    _handler = allocate_information_block( );
                    for ( auto i { 0ul }; i < MEMORY_PAGE_DESCRIPTOR; ++i ) {
                        _handler->bitmap[ i ] = { i, BITMAP_MASK };
                    }
                    _handler->flags.set_full( PAGE_FLAGS_FULL );
                    memset( physical_to_virtual( _handler->map_base_adderess ), 0, PAGE_4K_SIZE * MEMORY_PAGE_DESCRIPTOR );
                }
                // 最后一块
                _handler = allocate_information_block( );
                for ( auto i { 0ul }; i < ( size % MEMORY_PAGE_DESCRIPTOR ); ++i ) {
                    _handler->bitmap[ i ] = { i, BITMAP_MASK };
                }
                memset( physical_to_virtual( _handler->map_base_adderess ), 0, PAGE_4K_SIZE * ( size % MEMORY_PAGE_DESCRIPTOR ) );
            }
            lock.release( );
            return (VOID *)handler->map_base_adderess;
        }
        else if ( size < MEMORY_PAGE_DESCRIPTOR ) {
            if ( !memory_page_table_4k.is_empty( ) ) {
                handler = ( (PageHandler *)memory_page_table_4k.pop( )->container );
                memory_page_table_4k.push( handler->node );
                if ( handler->flags.flag_full( ) )     // 满了
                {
                    // 新建一块内存页信息块
                    handler = allocate_information_block( );
                }
                // 没有满，那么就用这个原来的块
            }
            else {
                // 整个1G内存队列都是空的，
                // 也就是第一次分配，那么就开辟一块空间存放信息

                handler = allocate_information_block( );
                // 然后进行出来
            }
            auto status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };
            auto index = handler->bitmap.allocate( size );
            VOID *buffer { };
            if ( index != -1 ) {
                for ( auto i { 0ul }; i < index + size; ++i ) {
                    handler->bitmap[ i ] = { i, 1 };
                }
                buffer = (VOID *)( handler->map_base_adderess + index * PAGE_4K_SIZE );
                memset( physical_to_virtual( buffer ), 0, size * PAGE_4K_SIZE );
            }

            if ( handler->bitmap.is_full( ) ) {
                // 如果满了，那就要把标志位设置一个1
                handler->flags.set_flags( PAGE_FLAGS_FULL );
            }
            Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
            lock.release( );
            return buffer;
        }
        return NULL;
    }
    auto MemoryPageMap4K::free( IN VOID * address, IN uint64_t size )->VOID {
        lock.acquire( );
        if ( !size || !address ) {
            lock.release( );
            return;
        }
        auto base = (uint64_t)address & ~( PAGE_4K_SIZE * MEMORY_PAGE_DESCRIPTOR );
        auto node = memory_page_table_4k.traversal(
            [ & ]( ListNode *node, uint64_t ) -> BOOL {
                return ( (PageHandler *)node->container )->map_base_adderess == base;
            },
            0 );
        if ( !node ) {
            lock.release( );
            return;
        }
        auto index = ( (uint64_t)address - base ) / PAGE_4K_SIZE;
        auto handler = (PageHandler *)node->container;
        auto status { Architecture::ArchitectureManager< TARGET_ARCH >::disable_interrupt( ) };

        for ( auto i { index }; i < index + size; ++i ) {
            handler->bitmap[ i ] = { i, 0 };
        }
        // 清空之前废弃的数据
        memset( physical_to_virtual( address ), 0, size * PAGE_4K_SIZE );
        Architecture::ArchitectureManager< TARGET_ARCH >::set_interrupt( status );
        lock.release( );
    }
}