#include <kernel/cpu/cpu.hpp>
#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    using namespace std;
    PageManager::PageManager( VOID ) noexcept {
        Ptr< limine_memmap_response > memory_descriptor { &__config.memory_map };

        __page_header__< PAGE_2M, PAGE_1G >::__helper__::get_group( ).init( );
        __page_header__< PAGE_1G, NONE >::__helper__::get_group( ).init( );
        __page_header__< PAGE_4K, PAGE_2M >::__helper__::get_group( ).init( );

        using PH  = __page_header__< PAGE_2M, PAGE_1G >;
        using PHI = PH::__helper__::__page_information__;
        uint64_t *info_address { };     // 找到一块空闲内存
        for ( auto i = 0ul; i < __config.memory_map.entry_count; ++i ) {
            if ( memory_descriptor->entries[ i ]->type == LIMINE_MEMMAP_USABLE ) {
                if ( memory_descriptor->entries[ i ]->base / PageAllocater::__page_size__< MemoryPageType::PAGE_2M > > 0 ) {
                    info_address = new ( physical_to_virtual( memory_descriptor->entries[ i ]->base ) ) uint64_t { };
                    break;
                }
            }
        }

        std::memset( (VOID *)info_address, 0, PageAllocater::__page_size__< MemoryPageType::PAGE_2M > );
        PH page_header { PageAllocater::__page_size__< MemoryPageType::PAGE_2M > / PH::__helper__::header_size, (uint64_t)info_address, 0ul };

        auto *bitmap = std::get< PHI >( page_header.get( 0 ) ).bitmap;
        bitmap->set( reinterpret_cast< uint64_t >( virtual_to_physical( info_address ) ) / PageAllocater::__page_size__< MemoryPageType::PAGE_2M > );
        bitmap->set( 0, 32 );

        for ( auto i = 0ul; i < __config.memory_map.entry_count; ++i ) {
            auto entry = memory_descriptor->entries[ i ];
            auto start { entry->base }, end { start + entry->length };
            this->all_memory_total = end;     // 这个是总内存
                                              // 可以使用的内存，可回收内存皆属于空闲内存
            switch ( entry->type ) {
            case LIMINE_MEMMAP_USABLE:                     // 未使用
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:           // ACPI可回收内存
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:     // 启动时服务可回收内存
                // 如果是这三种类型那么就计算空闲内存
                start = Lib::DIV_ROUND_UP( start, PageAllocater::__page_size__< MemoryPageType::PAGE_2M > );
                end   = end / PageAllocater::__page_size__< MemoryPageType::PAGE_2M >;
                // 统计空闲内存
                if ( end >= start ) {
                    this->free_memory_total += ( end - start ) * PageAllocater::__page_size__< MemoryPageType::PAGE_2M >;
                }

                break;
            case LIMINE_MEMMAP_RESERVED:               // 保留内存
            case LIMINE_MEMMAP_ACPI_NVS:               // ACPI NVS内存
            case LIMINE_MEMMAP_BAD_MEMORY:             // 错误内存
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:     // 模块文件内存
            case LIMINE_MEMMAP_FRAMEBUFFER:            // 显存占用的

                // 计算取得偏移的基地址
                auto &&base_address = ( (uint64_t)start - page_header.get( 0 ).first.base_address );

                // 计算取得所在区域的header的编号
                auto &&base_index = ( base_address & PH::__helper__::__zone_memory_mask__( ) ) / PH::__helper__::__zone_min_memory__;
                // 取得处于所在header的bitmap中的编号
                auto &&index = (base_address & PageAllocater::__page_mask__< PAGE_2M >) / PageAllocater::__page_size__< PAGE_2M > % PH::__helper__::page_descriptor_count;

                auto &[ header, bitmap ] = page_header.get( base_index );
                start                    = start / PageAllocater::__page_size__< MemoryPageType::PAGE_2M >;
                end                      = Lib::DIV_ROUND_UP( end, PageAllocater::__page_size__< MemoryPageType::PAGE_2M > );
                // 将这部分内存添加至bitmap
                bitmap.set( index, end - start );
                header.flags.state = PHI::__page_flags__::__page_state__::NORMAL;

                break;
            }
        }

        // auto i = PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 );
        // println< print_level::SYSTEM >( "OS Can Use Memory : {}MB", i );
        // PageWalker { }.free< MemoryPageType::PAGE_4K >( i, 1 );
        // i = PageWalker { }.allocate< MemoryPageType::PAGE_4K >( 1 );
        // println< print_level::SYSTEM >( "OS Can Use Memory : {}MB", i );
        // PageWalker { }.free< MemoryPageType::PAGE_4K >( i, 1 );

        println< print_level::SYSTEM >( "OS Can Use Memory : {}MB", this->free_memory_total / 1_MB );
    }
}