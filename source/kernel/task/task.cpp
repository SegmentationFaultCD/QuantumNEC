#include <kernel/display/print.hpp>
#include <kernel/task/schedule/MuQss.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <kernel/task/task.hpp>
namespace Task {
auto initialize_task( std::uint64_t core ) -> void {
    kernel_thread_lock.acquire( );
    auto main = new PCB;
    main->name.append( Library::format( "init{}", core ) );
    kernel_thread_lock.release( );

    main->PID = id_pool.get( );
    main->thread_group.push_back( PCB::Thread { } );
    auto &mthread = main->thread_group[ 0 ];
    using namespace Memory;
    using enum Memory::Page::Type;

    kernel_thread_lock.acquire( );
    mthread.kernel_stack = (std::uint64_t)Page::allocator< P4Kib > { }.allocate( main->kernel_stack_size / Page::allocator< P4Kib >::__page_size__ );
    mthread.user_stack = (std::uint64_t)Page::allocator< P2Mib > { }.allocate( main->user_stack_size / Page::allocator< P2Mib >::__page_size__ );
    kernel_thread_lock.release( );

    mthread.frame = (Interrupt::IDT::Frame *)physical_to_virtual( mthread.kernel_stack );

    std::construct_at( mthread.frame );
    main->running_thread = &mthread;
    main->page_table = nullptr;     // 为空说明默认使用内核页表
    main->cpu = core;

    kernel_thread_lock.acquire( );
    scheduler->running_queue.push_back( main );
    kernel_thread_lock.release( );
}
}     // namespace Task