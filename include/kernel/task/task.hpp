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
#include <lib/vector>
namespace Task {

struct PidPool {
    Library::bitset< 1024 > map;
    auto get( ) {
        return map.find< false >( );
    }
} inline id_pool;

struct Schedule;

struct PCB {
    constexpr static auto user_stack_size = 8_MB;
    constexpr static auto kernel_stack_size = 4_KB;

    constexpr static auto USER_STACK_TOP = 0x0000800000000000;
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

    // 多线程必备(用户线程)
    std::cxxvector< Thread > thread_group;

    Schedule *schedule;

    explicit PCB( void ) = default;

    explicit PCB( std::string_view _name, std::uint64_t entry_offset, std::uint64_t text_physical, std::uint64_t text_segment_length );

    auto save_context( Interrupt::IDT::Frame *frame ) -> PCB &;
    auto get_context( ) {
        return this->running_thread->frame;
    }
    auto activate( void ) -> void;
    template < typename T >
    auto create( T *entry ) -> void {
    }

    auto schedule_thread( ) -> PCB & {
        return *this;
        // 给线程用的
    }

    // 线程上下文
};

auto initialize_task( std::uint64_t core ) -> void;
}     // namespace Task
