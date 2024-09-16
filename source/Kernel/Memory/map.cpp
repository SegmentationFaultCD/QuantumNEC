#include <Kernel/Memory/map.hpp>
#include <Kernel/Memory/heap.hpp>
#include <Kernel/Memory/page.hpp>
#include <Arch/Arch.hpp>
#include <Lib/IO/Stream/iostream>
#include <Lib/STL/string>
#include <Kernel/task.hpp>
#include <cstddef>
#include <cstring>
using namespace QuantumNEC::Lib::Types;
using namespace QuantumNEC::Lib;
using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;
PUBLIC namespace {
    PRIVATE constexpr auto PMLE_IDX( auto address ) {
        return ( (Lib::Types::uint64_t)address >> 39 ) & 0x1FF;
    }
    PRIVATE constexpr auto PDPTE_IDX( auto address ) {
        return ( (Lib::Types::uint64_t)address >> 30 ) & 0x1FF;
    }
    PRIVATE constexpr auto PDE_IDX( auto address ) {
        return ( (Lib::Types::uint64_t)address >> 21 ) & 0x1FF;
    }
    PRIVATE constexpr auto PTE_IDX( auto address ) {
        return ( (Lib::Types::uint64_t)address >> 12 ) & 0x1FF;
    }
    PRIVATE constexpr auto FONT_FILE_OCCUPIES_PAGE { 16 };

    PRIVATE TaskLock lock { };
    PRIVATE Lib::Types::uint64_t *address { };

    Lib::Types::uint64_t pdp_entry_count { }, pml4e_entry_count { };
}

PUBLIC namespace QuantumNEC::Kernel {
    auto Table::set_table( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::uint64_t flags )->VOID {
        if ( flags & this->flags_p( ) ) {
            this->set_p( 1 );     // 该页存在
        }
        else {
            this->set_p( 0 );     // 该不页存在
        }
        if ( flags & this->flags_rw( ) ) {
            this->set_rw( 1 );     // 可读也可以写
        }
        else {
            this->set_rw( 0 );     // 只读
        }
        if ( flags & this->flags_us( ) ) {
            this->set_us( 1 );     // 用户页，权限低， 超级管理者和用户都可以用
        }
        else {
            this->set_us( 0 );     // 超级管理者页，权限高，超级管理者才能用
        }
        if ( flags & this->flags_pwt( ) ) {
            this->set_pwt( 1 );     // 页级直通写
        }
        else {
            this->set_pwt( 0 );     // 页线回写
        }
        if ( flags & this->flags_pcd( ) ) {
            this->set_pcd( 1 );     // 开启页级
        }
        else {
            this->set_pcd( 0 );     // 不开启页级
        }
        if ( flags & this->flags_a( ) ) {
            this->set_a( 1 );     // 访问由MMU控制
        }
        else {
            this->set_a( 0 );     // 访问与MMU无关
        }
        if ( flags & this->flags_d( ) ) {
            this->set_d( 1 );     // 该内存页被占用
        }
        else {
            this->set_d( 0 );     // 该内存页未被占用
        }
        if ( flags & this->flags_pat_ps( ) ) {     // 页属性或者页大小
            this->set_ps_pat( 1 );
        }
        else {
            this->set_ps_pat( 0 );
        }
        if ( flags & this->flags_g( ) ) {     // 全局标志
            this->set_g( 1 );
        }
        else {
            this->set_g( 0 );
        }
        if ( flags & this->flags_avl( ) ) {
            this->set_avl( 1 );
        }
        else {
            this->set_avl( 0 );
        }
        if ( flags & this->flags_xd( ) ) {
            this->set_xd( 1 );     // 屏蔽
        }
        else {
            this->set_xd( 0 );     // 不屏蔽
        }
        this->set_address( reinterpret_cast< Lib::Types::uint64_t >( address ) );     // 把地址丢进去
    }
    auto PageDirectoryTable::make( IN Lib::Types::uint64_t flags, IN MemoryPageType type ) noexcept -> VOID {
        auto page_size { 0ull };     // 决定address_要加上多少
        switch ( type ) {
        case MemoryPageType::PAGE_4K:
            page_size = PAGE_4K_SIZE;
            break;
        case MemoryPageType::PAGE_2M:
            page_size = PAGE_2M_SIZE;
            break;
        case MemoryPageType::PAGE_1G:
            page_size = PAGE_1G_SIZE;
            break;
        }
        Lib::STL::memset( this->page_directory_table, 0, 4096 );
        for ( auto i { 0ull }; i < 512; ++i ) {
            this->page_directory_table[ i ].set_table( address, flags | ( 1 << 7 ) );
            address += page_size / sizeof *address;
        }
    }
    auto PageDirectoryPointerTable::make( IN Lib::Types::uint64_t flags, IN MemoryPageType type ) noexcept -> VOID {
        Lib::STL::memset( this->page_directory_pointer_table, 0, 4096 );
        for ( auto i { 0ull }; i < pdp_entry_count; ++i ) {
            this->page_dircetory_table[ i ].make( flags, type );
            this->page_directory_pointer_table[ i ].set_table( &this->page_dircetory_table[ i ], flags );
        }
    }
    auto PageMapLevel4Table::make( IN Lib::Types::uint64_t flags, IN MemoryPageType type ) noexcept -> VOID {
        Lib::STL::memset( this->page_map_level4_table, 0, 4096 );
        for ( auto i { 0ull }; i < pml4e_entry_count; ++i ) {
            this->page_directory_pointer_table[ i ].make( flags, type );
            this->page_map_level4_table[ i ].set_table( &this->page_directory_pointer_table[ i ], flags );
        }
        this->page_map_level4_table[ 0x100 ].set_table( &this->page_directory_pointer_table[ 0 ], flags );
    }

    PageMapLevel4Table::PageMapLevel4Table( VOID ) noexcept {
        // Lib::Types::uint8_t physical_address_bits { };
        // auto tmp { 0u }, eax { 0u };
        // Architecture::ArchitectureManager< TARGET_ARCH >::cpuid( 0x80000000, 0, &eax, &tmp, &tmp, &tmp );
        // if ( eax >= 0x80000008 ) {
        //     Architecture::ArchitectureManager< TARGET_ARCH >::cpuid( 0x80000008, 0, &eax, &tmp, &tmp, &tmp );
        //     physical_address_bits = eax;
        // }
        // else {
        //     physical_address_bits = 36;
        // }
        // if ( physical_address_bits > 48 ) {
        //     physical_address_bits = 48;
        // }
        // if ( physical_address_bits > 39 ) {
        //     pml4e_entry_count = 1 << ( physical_address_bits - 39 );
        //     physical_address_bits = 39;
        // }
        // pdp_entry_count = 1 << ( physical_address_bits - 30 );
        pdp_entry_count = 512;
        pml4e_entry_count = 1;
    }
    PageMapLevel4Table::PageMapLevel4Table( IN PageMapLevel4Table * pml4_t ) noexcept {
        this->page_map_level4_table[ 0x100 ] = pml4_t->page_map_level4_table[ 0x100 ];
    }
    auto PageMapLevel4Table::get_table_entry( IN Lib::Types::Ptr< VOID > address, IN MapLevel level )
        ->Lib::Types::Ptr< Lib::Types::uint64_t > {
        auto get_pmlt_entry = [ &, this ] {
            return reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint64_t > >( ( Lib::Types::uint64_t )( this ) + PMLE_IDX( address ) );
        };
        auto get_pdpt_entry = [ &, this ] {
            return reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint64_t > >( ( *get_pmlt_entry( ) & ~0xfff ) + PDPTE_IDX( address ) );
        };
        auto get_pd_entry = [ &, this ] {
            return reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint64_t > >( ( *get_pdpt_entry( ) & ~0xfff ) + PDE_IDX( address ) );
        };
        Lib::Types::Ptr< Lib::Types::uint64_t > entry { };
        using enum MapLevel;
        switch ( level ) {
        case PML:
            entry = get_pmlt_entry( );
            break;
        case PDPT:
            entry = get_pdpt_entry( );
            break;
        case PD:
            entry = get_pd_entry( );
            break;
        case PT:
            entry = reinterpret_cast< Lib::Types::Ptr< Lib::Types::uint64_t > >( ( *get_pd_entry( ) & ~0xfff ) + Lib::Base::OFFSET( address ) );
            break;
        default:
            entry = NULL;
            break;
        }
        return entry;
    }
    auto PageMapLevel4Table::check( IN Lib::Types::Ptr< Lib::Types::uint64_t > virtual_address )->Lib::Types::BOOL {
        if ( ( *virtual_address >> 47 ) & 1 ) {
            if ( *virtual_address >> 48 == 0xFFFF ) {
                return TRUE;
            }
            *virtual_address |= 0xFFFFULL << 48;
            return FALSE;
        }
        return TRUE;
    }
    MemoryMap::MemoryMap( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept {
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Start mapping the page table." << Lib::IO::endl;
        // 消除页保护
        this->page_table_protect( FALSE );
        this->kernel_page_table = new ( reinterpret_cast< VOID * >( KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS ) ) PageMapLevel4Table { };
        this->kernel_page_table->make( PAGE_PRESENT | PAGE_RW_W | PAGE_US_U, MemoryPageType::PAGE_2M );
        // 挂载页表
        Architecture::ArchitectureManager< TARGET_ARCH >::set_page_table( reinterpret_cast< Lib::Types::uint64_t * >( this->kernel_page_table ) );

        // 映射
        /*
         * 系统内存分配:
         * · 0x0000000  ->  0x00fffff = [1MB]            --> 内核起始点
         * · 0x0100000  ->  0x01fffff = [1MB]            --> 内核
         * · 0x0200000  ->  0x020ffff = [64KB]           --> 内核PCB
         * · 0x0210000  ->  0x03fffff = [2MB]            --> 内核保留
         * · 0x0400000  ->  0x23fffff = [32MB]           --> 字体文件
         * · 0x2400000  ->  0x2400fff = [4 KB]           --> 保留
         * · 0x2401000  ->  0x2401fff = [4 KB]           --> 保留
         * · 0x2412000  ->  0x24fffff = [1MB]            --> 保留
         * · 0x2500000  ->  ......... = [X MB]           --> 内核页表
         * · 内核页表结尾 ->  .........   [32MB + X MB之后] --> 空闲
         * 0x0000000000000000 - 0x00000000ffffffff ==> 0xffff800000000000 - 0xffff8000ffffffff
         * 0xffff800000000000 -> 0xffff8000000fffff      = [1MB]            --> 内核起始点
         * 0xffff800000100000 -> 0xffff8000001fffff      = [1MB]            --> 内核
         * 0xffff800000200000 -> 0xffff80000020ffff      = [4KB]            --> 内核PCB
         * 0xffff800000210000 -> 0xffff8000003fffff      = [2MB]            --> 显存
         * 0xffff800000400000 -> 0xffff8000023fffff      = [32MB]           --> 字体
         * 0xffff800002400000 -> 0xffff800002410fff      = [4KB]            --> I-O APIC 所占用
         * 0xffff800002411000 -> 0xffff800002411fff      = [4KB]            --> Local APIC 所占用
         * 0xffff800002412000 -> 0xffff8000024fffff      = [1MB]            --> 保留
         * 0xffff800002500000 -> ..................      = [X MB]           --> 内核页表
         * 内核页表结尾         -> ..................      = [32MB + X MB之后] --> 空闲
         */

        KERNEL_VRAM_PHYSICAL_ADDRESS = _config->graphics_data.FrameBufferBase;
        KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS = KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS + this->kernel_page_table->size( );

        KERNEL_FREE_MEMORY_VIRTUAL_ADDRESS = KERNEL_BASE_ADDRESS + KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS;
        KERNEL_I_O_APIC_PHYSICAL_ADDRESS = reinterpret_cast< Lib::Types::uint64_t >( Architecture::ArchitectureManager< TARGET_ARCH >::io_apic_address );
        KERNEL_LOCAL_APIC_PHYSICAL_ADDRESS = reinterpret_cast< Lib::Types::uint64_t >( Architecture::ArchitectureManager< TARGET_ARCH >::local_apic_address );
        // 映射内核起始地址
        this->map( 0, KERNEL_BASE_ADDRESS, 1, PAGE_PRESENT | PAGE_RW_R | PAGE_US_S | PAGE_DIRTY | PAGE_ACCESSED, MapMode::MEMORY_MAP_2M );
        // 映射内核
        this->map( KERNEL_PHYSICAL_ADDRESS, KERNEL_VIRTUAL_ADDRESS, 1, PAGE_PRESENT | PAGE_RW_R | PAGE_US_S | PAGE_DIRTY | PAGE_ACCESSED, MapMode::MEMORY_MAP_2M );
        // 映射内核PCB
        this->map( KERNEL_TASK_PCB_PHYSICAL_ADDRESS, KERNEL_TASK_PCB_VIRTUAL_ADDRESS, 21, PAGE_PRESENT | PAGE_RW_W | PAGE_US_S, MapMode::MEMORY_MAP_4K );
        // 映射内核IO apic与Local apic所占用内存
        this->map( KERNEL_I_O_APIC_PHYSICAL_ADDRESS, KERNEL_I_O_APIC_VIRTUAL_ADDRESS, 1, PAGE_PRESENT | PAGE_RW_W | PAGE_US_S, MapMode::MEMORY_MAP_4K );
        this->map( KERNEL_LOCAL_APIC_PHYSICAL_ADDRESS, KERNEL_LOCAL_APIC_VIRTUAL_ADDRESS, 1, PAGE_PRESENT | PAGE_RW_W | PAGE_US_S, MapMode::MEMORY_MAP_4K );
        // 映射显存
        this->map( KERNEL_VRAM_PHYSICAL_ADDRESS, KERNEL_VRAM_VIRTUAL_ADDRESS, 1, PAGE_PRESENT | PAGE_RW_W | PAGE_US_U, MapMode::MEMORY_MAP_2M );
        // 映射字体
        this->map( KERNEL_FONT_MEMORY_PHYSICAL_ADDRESS, KERNEL_FONT_MEMORY_VIRTUAL_ADDRESS, FONT_FILE_OCCUPIES_PAGE, PAGE_US_U | PAGE_RW_W | PAGE_PRESENT, MapMode::MEMORY_MAP_2M );

        // 映射页表以及空闲内存
        for ( auto page_address { KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS }; page_address < Memory::page->memory_total; page_address += PAGE_SIZE ) {
            // 从空闲内存起始地址开始映射
            this->map( page_address, KERNEL_BASE_ADDRESS + page_address, 1, PAGE_US_U | PAGE_RW_W | PAGE_PRESENT, MapMode::MEMORY_MAP_2M );
        }

        // 剔除被占用的内存(0 -> 32MB + pagetable size MB)
        for ( auto index { 0ull }; index < KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS / PAGE_SIZE + 1; ++index ) {
            Memory::page->bitmap_.set( index, 1 );
        }
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Mapping the pages table is OK." << Lib::IO::endl;
    }
    auto MemoryMap::map( IN Lib::Types::uint64_t physics_address, IN Lib::Types::uint64_t virtual_address, IN Lib::Types::size_t size, IN Lib::Types::uint16_t flags, IN MapMode, IN Lib::Types::Ptr< PageMapLevel4Table > pml )->VOID {
        lock.acquire( );
        physics_address = physics_address & ~( PAGE_SIZE * size - 1 );
        virtual_address = virtual_address & ~( PAGE_SIZE * size - 1 );
        pml_t virtual_pml { }, virtual_pmle { };
        pdpt_t virtual_pdpt { }, physical_pdpt { }, virtual_pdpte { }, physical_pdpte { };
        pdt_t virtual_pdt { }, physical_pdt { }, virtual_pde { }, physical_pde { };
        virtual_pml.set_mplt( reinterpret_cast< Lib::Types::uint64_t * >( pml ) );
        virtual_pmle.set_mplt( virtual_pml.pml + PMLE_IDX( virtual_address ) );
        if ( !( *virtual_pmle.pml & PAGE_PRESENT ) ) {
            physical_pdpt.set_pdpt( reinterpret_cast< decltype( physical_pdpt.pdpt ) >( KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS ) );
            KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS += PT_SIZE;
            virtual_pdpt.set_pdpt( reinterpret_cast< decltype( virtual_pdpt.pdpt ) >( ( physical_pdpt.pdpt ) ) );
            Lib::STL::memset( virtual_pdpt.pdpt, 0, PT_SIZE );
            virtual_pmle.make_mplt( physical_pdpt.pdpt, flags );
        }
        physical_pdpt.set_pdpt( reinterpret_cast< decltype( physical_pdpt.pdpt ) >( *virtual_pmle.pml & ( ~0xfff ) ) );
        physical_pdpte.set_pdpt( physical_pdpt.pdpt + PDPTE_IDX( virtual_address ) );
        virtual_pdpte.set_pdpt( reinterpret_cast< decltype( virtual_pdpte.pdpt ) >( ( physical_pdpte.pdpt ) ) );
        if ( !( *virtual_pdpte.pdpt & PAGE_PRESENT ) ) {
            physical_pdt.set_pdt( reinterpret_cast< decltype( physical_pdt.pdt ) >( KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS ) );
            KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS += PT_SIZE;
            virtual_pdt.set_pdt( reinterpret_cast< decltype( virtual_pdt.pdt ) >( ( physical_pdt.pdt ) ) );
            Lib::STL::memset( virtual_pdt.pdt, 0, PT_SIZE );
            virtual_pdpte.make_pdpt( physical_pdt.pdt, flags );
        }
        physical_pdt.set_pdt( reinterpret_cast< decltype( physical_pdt.pdt ) >( *virtual_pdpte.pdpt & ( ~0xfff ) ) );
        physical_pde.set_pdt( physical_pdt.pdt + PDE_IDX( virtual_address ) );
        virtual_pde.set_pdt( reinterpret_cast< decltype( physical_pdt.pdt ) >( ( physical_pde.pdt ) ) );
        virtual_pde.make_pdt( reinterpret_cast< decltype( virtual_pde.pdt ) >( physics_address ), flags | ( 1 << 7 ) );
        Architecture::ArchitectureManager< TARGET_ARCH >::invlpg( reinterpret_cast< Lib::Types::Ptr< VOID > >( virtual_address ) );
        lock.release( );
    }
    auto MemoryMap::unmap( IN Lib::Types::uint64_t virtual_address, IN Lib::Types::size_t size, IN Lib::Types::Ptr< PageMapLevel4Table > pml )->VOID {
        /*
         * 内容大致与map一至，除了取消映射的地方和判断
         */
        lock.acquire( );
        virtual_address = virtual_address & ~( PAGE_SIZE * size - 1 );
        pml_t virtual_pml { }, virtual_pmle { };
        pdpt_t physical_pdpt { }, virtual_pdpte { }, physical_pdpte { };
        pdt_t virtual_pde { }, physical_pdt { }, physical_pde { };
        virtual_pml.set_mplt( reinterpret_cast< decltype( virtual_pml.pml ) >( ( pml ) ) );
        virtual_pmle.set_mplt( virtual_pml.pml + PMLE_IDX( virtual_address ) );
        if ( !( *virtual_pmle.pml & PAGE_PRESENT ) ) {
            return;
        }
        physical_pdpt.set_pdpt( reinterpret_cast< decltype( physical_pdpt.pdpt ) >( *virtual_pmle.pml & ( ~0xFFF ) ) );
        physical_pdpte.set_pdpt( physical_pdpt.pdpt + PDPTE_IDX( virtual_address ) );
        virtual_pdpte.set_pdpt( reinterpret_cast< decltype( virtual_pdpte.pdpt ) >( ( physical_pdpte.pdpt ) ) );
        if ( !( *virtual_pdpte.pdpt & PAGE_PRESENT ) ) {
            return;
        }
        physical_pdt.set_pdt( reinterpret_cast< decltype( physical_pdt.pdt ) >( *virtual_pdpte.pdpt & ( ~0xFFF ) ) );
        physical_pde.set_pdt( physical_pdt.pdt + PDE_IDX( virtual_address ) );
        virtual_pde.set_pdt( reinterpret_cast< decltype( physical_pdt.pdt ) >( ( physical_pde.pdt ) ) );
        *virtual_pde.pdt &= ~PAGE_PRESENT;
        lock.release( );
    }

    auto MemoryMap::page_table_protect( IN Lib::Types::BOOL flags )->VOID {
        auto cr0 = Architecture::ArchitectureManager< TARGET_ARCH >::read_cr0( );
        if ( !flags ) {
            cr0.WP = 1;
            Architecture::ArchitectureManager< TARGET_ARCH >::write_cr0( cr0 );
            Lib::IO::sout[ Lib::IO::ostream::HeadLevel::SYSTEM ] << "Disable the page protection." << Lib::IO::endl;
        }
        else {
            cr0.WP = 0;
            Architecture::ArchitectureManager< TARGET_ARCH >::write_cr0( cr0 );
            Lib::IO::sout[ Lib::IO::ostream::HeadLevel::SYSTEM ] << "Enable the page protection." << Lib::IO::endl;
        }
    };
    auto MemoryMap::make_page_table( VOID )->Lib::Types::Ptr< PageMapLevel4Table > {
        // Lib::IO::sout << (void *)*( (unsigned long long *)kernel_page_table ) << '\n';
        PageMapLevel4Table *pml4t = new ( new Lib::Types::byte_t[ PT_SIZE ] ) PageMapLevel4Table { kernel_page_table };
        // Lib::Types::uint64_t *pml4t = new ( HeapMemory::malloc( PT_SIZE ) ) Lib::Types::uint64_t { };
        // Lib::STL::memcpy( pml4t + 0x100, (unsigned long long *)kernel_page_table + 0x100, 2048 );
        return pml4t;
    }
    auto MemoryMap::activate_page_table( IN Lib::Types::Ptr< VOID > )->VOID {
        //  Architecture::ArchitectureManager< TARGET_ARCH >::set_page_table( page_directory_table_address ? reinterpret_cast< Lib::Types::uint64_t * >( page_directory_table_address ) : reinterpret_cast< Lib::Types::uint64_t * >( kernel_page_table ) );
    }
    auto MemoryMap::get_current_page_tabel( VOID )->Lib::Types::Ptr< Lib::Types::uint64_t > {
        return reinterpret_cast< Lib::Types::uint64_t * >( Architecture::ArchitectureManager< TARGET_ARCH >::read_cr3( ).page_directory_base );
    }
}