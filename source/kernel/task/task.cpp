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

    main->schedule = new Schedule;
    main->schedule->hw_scheduler = scheduler;

    kernel_thread_lock.acquire( );
    main->schedule->hw_scheduler->running_queue.push_back( { } );
    kernel_thread_lock.release( );
}
PCB::PCB( std::string_view _name, auto entry, std::uint64_t text_segment_length ) :
    name { _name },
    page_table { new Memory::Paging::pml4t {} }, thread_group { }, PID { id_pool.get( ) } {
    using namespace Memory;
    this->page_table->copy( *paging->kernel_page_table );
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
    mthread.frame->rip = (void *)entry;
    this->page_table->map( mthread.user_stack,
                           this->USER_STACK_START_ADDRESS - this->user_stack_size,
                           this->user_stack_size / Page::allocator< P2Mib >::__page_size__,
                           this->page_table->PAGE_PRESENT | this->page_table->PAGE_RW_W | this->page_table->PAGE_US_U | this->page_table->PAGE_XD,
                           Page::Type::P2Mib );
    mthread.frame->rsp = USER_STACK_START_ADDRESS;
    mthread.frame->rflags.IOPL = 0;
    mthread.frame->rflags.MBS = 1;
    mthread.frame->rflags.IF = 1;

    this->running_thread = &mthread;

    this->schedule = new Schedule;
    this->schedule->hw_scheduler = scheduler;
}
}     // namespace Task