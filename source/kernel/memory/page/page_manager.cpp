#include <kernel/memory/page/page_manager.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    using namespace std;
    PageManager::PageManager( VOID ) noexcept {
        allocate_information_list[ 1 ].init( );
        allocate_information_list[ 2 ].init( );
        allocate_information_list[ 3 ].init( );
        Ptr< limine_memmap_response > memory_descriptor { &__config.memory_map };
        using PH = __page_header< PAGE_2M, PAGE_2M, PAGE_1G >;
        using PHI = PH::__page_information;
        uint64_t *info_address { };     // 找到一块空闲内存
        for ( auto i = 0ul; i < __config.memory_map.entry_count; ++i ) {
            if ( memory_descriptor->entries[ i ]->type == LIMINE_MEMMAP_USABLE ) {
                if ( memory_descriptor->entries[ i ]->base / PageAllocater::__page_size< MemoryPageType::PAGE_2M > > 0 ) {
                    info_address = new ( physical_to_virtual( memory_descriptor->entries[ i ]->base ) ) uint64_t { };
                    break;
                }
            }
        }

        PH page_header { 1, { (uint64_t)info_address, 0 }, { 0, 0 } };
        auto *bitmap = std::get< PHI >( page_header.get( 0 ) ).bitmap;
        bitmap->set( reinterpret_cast< uint64_t >( virtual_to_physical( info_address ) ) / PageAllocater::__page_size< MemoryPageType::PAGE_2M > );
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
                start = Lib::DIV_ROUND_UP( start, PageAllocater::__page_size< MemoryPageType::PAGE_2M > );
                end = end / PageAllocater::__page_size< MemoryPageType::PAGE_2M >;
                // 统计空闲内存
                if ( end >= start ) {
                    this->free_memory_total += ( end - start ) * PageAllocater::__page_size< MemoryPageType::PAGE_2M >;
                }

                break;
            case LIMINE_MEMMAP_RESERVED:               // 保留内存
            case LIMINE_MEMMAP_ACPI_NVS:               // ACPI NVS内存
            case LIMINE_MEMMAP_BAD_MEMORY:             // 错误内存
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:     // 模块文件内存
            case LIMINE_MEMMAP_FRAMEBUFFER:            // 显存占用的
                // 如果是以上这几类，那么将其所在的bitmap中的位置设置为1
                auto group_base_address = (uint64_t)start & ~( PageAllocater::__page_size< MemoryPageType::PAGE_2M > * PH::page_descriptor_count * PH::page_header_count - 1 );
                // 计算取得所在组的块的编号
                auto base_index = ( (uint64_t)start - group_base_address ) / ( PH::page_descriptor_count * PageAllocater::__page_size< MemoryPageType::PAGE_2M > );
                // 取得处于所在组的块的bitmap中的编号
                auto index = (((uint64_t)start & PageAllocater::__page_mask< MemoryPageType::PAGE_2M >) / PageAllocater::__page_size< MemoryPageType::PAGE_2M >) % PH::page_descriptor_count;
                auto &[ header, bitmap ] = page_header.get( base_index );
                start = start / PageAllocater::__page_size< MemoryPageType::PAGE_2M >;
                end = Lib::DIV_ROUND_UP( end, PageAllocater::__page_size< MemoryPageType::PAGE_2M > );
                // 将这部分内存添加至bitmap
                bitmap.set( index, end - start );
                header.flags.state = PH::NORMAL;
                break;
            }
        }
        println< ostream::HeadLevel::SYSTEM >( "OS Can Use Memory : {}MB", this->free_memory_total / 1_MB );
    }
}