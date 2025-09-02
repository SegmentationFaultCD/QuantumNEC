#pragma once
#include <cstdint>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/allocator/kheap.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/page_table.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <lib/format.hpp>
#include <vector>
namespace Task {

struct PidPool {
    Library::bitset< 1024 > map;
    auto get( ) {
        return map.find< false >( );
    }
} inline id_pool;

class PCB {
    constexpr static auto user_stack_size = 8_MB;
    constexpr static auto kernel_stack_size = 4_KB;
    Library::cxxstring name;
    std::uint64_t PID;
    struct Thread {
        std::uint64_t user_stack;
        std::uint64_t kernel_stack;
        Interrupt::IDT::Frame *frame;
    } mthread;

    Thread *running_thread;

    Memory::Paging::pmlxt *page_table;
    std::vector< Thread, Memory::KernelHeap::allocator< Thread > > thread_group;

    template < typename T >
    explicit PCB( std::string_view _name, T *entry ) {
        this->name = _name;
        using namespace Memory;

        this->page_table = new Paging::pml4t { };
        this->page_table->copy( *Paging::kernel_page_table );

        this->thread_group.emplace_back( mthread );
        this->PID = id_pool.get( );
        using enum Memory::Page::Type;

        mthread.kernel_stack = (std::uint64_t)Page::allocator< P4Kib > { }.allocate( kernel_stack_size / Page::allocator< P4Kib >::__page_size__ );
        mthread.user_stack = (std::uint64_t)Page::allocator< P2Mib > { }.allocate( user_stack_size / Page::allocator< P2Mib >::__page_size__ );

        mthread.frame = (Interrupt::IDT::Frame *)physical_to_virtual( mthread.kernel_stack + kernel_stack_size - sizeof *mthread.frame );
        mthread.frame->cs = GDT::SELECTOR_CODE64_USER;
        mthread.frame->ss = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.ds = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.es = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.fs = GDT::SELECTOR_DATA64_USER;
        mthread.frame->regs.gs = GDT::SELECTOR_DATA64_USER;
        mthread.frame->rip = entry;
        mthread.frame->rsp = physical_to_virtual( mthread.user_stack + user_stack_size );
        mthread.frame->rflags.IOPL = 0;
        mthread.frame->rflags.MBS = 1;
        mthread.frame->rflags.IF = 1;

        this->running_thread = &mthread;
    }
    template < typename T >
    auto create( T *entry ) -> void {
    }

    auto schedule( ) {
        return mthread.frame;
        // 给线程用的
    }

    // 线程上下文
};

}     // namespace Task
