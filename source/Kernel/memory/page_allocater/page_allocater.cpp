#include "Kernel/memory/page_allocater/page_allocater.hpp"
#include "Kernel/memory/paging_map/ptv.hpp"
#include <Kernel/memory/memory.hpp>
#include <Kernel/print.hpp>
#include <Libcxx/string.hpp>
#include <Lib/deflib.hpp>
#include <Arch/Arch.hpp>
#include <Lib/spin_lock.hpp>
PUBLIC namespace {
    using namespace QuantumNEC;
    using namespace QuantumNEC::Kernel;
    using namespace QuantumNEC::Lib;
    using namespace std;
    PRIVATE SpinLock lock { };
}

PUBLIC namespace QuantumNEC::Kernel {
    PageAllocater::PageAllocater( VOID ) noexcept {     // 计算可用内存数量

        Ptr< limine_memmap_response > memory_descriptor { &Architecture::__config.memory_map };

        using PH = PageHeader< PageAllocater2M, PageAllocater2M >;
        using PHI = PH::PageInformation;

        PH page_header { this->p2m, physical_to_virtual( nullptr ), MemoryPageType::PAGE_2M };
        auto bitmap = std::get< PHI >( page_header.get( 0 ) ).bitmap_;
        bitmap->set( 0 );
        for ( auto i = 0ul; i < Architecture::__config.memory_map.entry_count; ++i ) {
            auto entry = memory_descriptor->entries[ i ];
            auto start { entry->base }, end { start + entry->length };
            this->all_memory_total = end;     // 这个是总内存
                                              // 可以使用的内存，可回收内存皆属于空闲内存
            switch ( entry->type ) {
            case LIMINE_MEMMAP_USABLE:                     // 未使用
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:           // ACPI可回收内存
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:     // 启动时服务可回收内存
                // 如果是这三种类型那么就计算空闲内存
                start = DIV_ROUND_UP( start, PAGE_2M_SIZE );
                end = end / PAGE_2M_SIZE;
                // 统计空闲内存
                if ( end >= start ) {
                    this->free_memory_total += ( end - start ) * PAGE_2M_SIZE;
                }
                break;

            case LIMINE_MEMMAP_RESERVED:               // 保留内存
            case LIMINE_MEMMAP_ACPI_NVS:               // ACPI NVS内存
            case LIMINE_MEMMAP_BAD_MEMORY:             // 错误内存
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:     // 模块文件内存
            case LIMINE_MEMMAP_FRAMEBUFFER:            // 显存占用的
                // 如果是以上这几类，那么将其所在的bitmap中的位置设置为1
                start = start / PAGE_2M_SIZE;
                end = DIV_ROUND_UP( end, PAGE_2M_SIZE );
                // 将这部分内存添加至bitmap
                bitmap->set( start, end - start );
                break;
            }
        }

        println< ostream::HeadLevel::SYSTEM >( "OS Can Use Memory : {}MB", this->free_memory_total / 1_MB );
    }
    auto PageAllocater::allocate( IN size_t size, IN MemoryPageType type ) -> VOID * {
        lock.acquire( );
        using enum MemoryPageType;
        VOID *buffer { };
        switch ( type ) {
        case PAGE_1G:
            buffer = this->p1g.allocate( size );
            break;
        case PAGE_2M:
            buffer = this->p2m.allocate( size );
            break;
        case PAGE_4K:
            buffer = this->p4k.allocate( size );
            break;
        }
        lock.release( );
        return buffer;
    }
    auto PageAllocater::free( IN VOID * address, IN size_t size, IN MemoryPageType type ) -> VOID {
        lock.acquire( );
        using enum MemoryPageType;
        switch ( type ) {
        case PAGE_1G:
            this->p1g.free( address, size );
            break;
        case PAGE_2M:
            this->p2m.free( address, size );
            break;
        case PAGE_4K:
            this->p4k.free( address, size );
            break;
        }
        lock.release( );
    }
}