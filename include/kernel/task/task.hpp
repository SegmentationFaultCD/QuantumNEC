#pragma once
#include <cstdint>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/allocator/kheap.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/page_table.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <lib/format.hpp>
#include <vector>
namespace Task {
class PCB;
inline Scheduler< PCB > *scheduler;

struct PidPool {
    Library::bitset< 1024 > map;
    auto get( ) {
        return map.find< false >( );
    }
} inline id_pool;

class PCB {
    friend auto initialize_task( std::uint64_t core ) -> void;

    constexpr static auto user_stack_size = 8_MB;
    constexpr static auto kernel_stack_size = 4_KB;

    constexpr static auto USER_STACK_START_ADDRESS = 0x8000000000000000ul;
    constexpr static auto TEXT_SEGMENT = 0x08048000;
    constexpr static auto SHARED_LIBRARY_SEGMENT = 0x1000000000000000ul;

    std::cxxstring name;
    std::int64_t PID;

    struct Thread {
        std::uint64_t user_stack;
        std::uint64_t kernel_stack;
        Interrupt::IDT::Frame *frame;
    };

    Thread *running_thread;

    std::unique_ptr< Memory::Paging::pmlxt > page_table;

    std::vector< Thread, Memory::KernelHeap::allocator< Thread > > thread_group;

    std::uint64_t cpu;

public:
    explicit PCB( void ) = default;

    explicit PCB( std::string_view _name, auto *entry, std::uint64_t text_segment_length ) :
        name { _name },
        page_table { new Memory::Paging::pml4t {} }, thread_group { }, PID { id_pool.get( ) } {
        using namespace Memory;
        this->page_table->copy( *Paging::kernel_page_table );
        using enum Memory::Page::Type;
        thread_group.push_back( Thread { } );
        auto &mthread = thread_group[ 0 ];
        mthread.kernel_stack = (std::uint64_t)Page::allocator< P4Kib > { }.allocate( this->kernel_stack_size / Page::allocator< P4Kib >::__page_size__ );
        mthread.user_stack = (std::uint64_t)Page::allocator< P2Mib > { }.allocate( this->user_stack_size / Page::allocator< P2Mib >::__page_size__ );
        mthread.frame = (Interrupt::IDT::Frame *)physical_to_virtual( mthread.kernel_stack );     // 内核栈栈底
        std::construct_at( mthread.frame );
        mthread.frame->cs = GDT::SELECTOR_CODE64_USER;
        mthread.frame->ss = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.ds = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.es = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.fs = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.gs = GDT::SELECTOR_DATA64_USER;
        this->page_table->map( (std::uint64_t)entry,
                               this->TEXT_SEGMENT,
                               ( text_segment_length + ( 4_KB - 1 ) ) / 4_KB,
                               this->page_table->PAGE_PRESENT | this->page_table->PAGE_RW_W | this->page_table->PAGE_US_U,
                               Page::Type::P2Mib );
        mthread.frame->rip = entry;
        this->page_table->map( mthread.user_stack,
                               this->USER_STACK_START_ADDRESS - this->user_stack_size,
                               this->user_stack_size / Page::allocator< P2Mib >::__page_size__,
                               this->page_table->PAGE_PRESENT | this->page_table->PAGE_RW_W | this->page_table->PAGE_US_U | this->page_table->PAGE_XD,
                               Page::Type::P2Mib );
        mthread.frame->rsp = USER_STACK_START_ADDRESS;
        mthread.frame->rflags.IOPL = 0;
        mthread.frame->rflags.MBS = 1;
        mthread.frame->rflags.IF = 1;
        this->cpu = Interrupt::apic.apic_id( );
        this->running_thread = &mthread;
    }
    template < typename T >
    auto create( T *entry ) -> void {
    }

    auto schedule( ) {
        return thread_group[ 0 ].frame;
        // 给线程用的
    }

    // 线程上下文
};

auto initialize_task( std::uint64_t core ) -> void;
}     // namespace Task
