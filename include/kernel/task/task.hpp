#pragma once
#include <cstdint>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/allocator/kheap.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/page_table.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <lib/format.hpp>
#include <lib/vector>
namespace Task {

struct PidPool {
    std::bitset< 1024 > map;
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

    std::unique_ptr< Memory::Paging::pmlxt > page_table;

    struct Thread {
        std::uint64_t user_stack;
        std::uint64_t kernel_stack;
        Interrupt::IDT::Frame *frame;
    };

    Thread *running_thread;

    // 多线程必备(用户线程,调度方式由进程定义)
    std::cxxvector< Thread > thread_group;

    std::unique_ptr< Schedule > schedule;

    auto save_context( Interrupt::IDT::Frame *frame ) -> PCB &;

    auto get_context( ) {
        return this->running_thread->frame;
    }

    explicit PCB( void ) = default;

    explicit PCB( std::string_view _name, std::uint64_t entry_offset, std::uint64_t text_physical, std::uint64_t text_segment_length );
    PCB( PCB &&p );

    explicit PCB( const PCB & ) = delete;
    ~PCB( ) = default;
    auto operator=( PCB && ) -> PCB &;
    auto operator=( const PCB & ) -> PCB & = delete;
    auto operator!=( const PCB &t ) {
        return name != t.name || PID != t.PID;
    }
    auto activate( void ) -> void;
    template < typename T >
    auto create( T *entry ) -> void {
    }

    auto schedule_thread( ) -> PCB & {
        return *this;
        // 给线程用的
    }
    auto has_task( ) {
        return !this->is_empty;
    }

private:
    bool is_empty = true;
    // 线程上下文
};

auto initialize_task( std::uint64_t core ) -> void;
}     // namespace Task
