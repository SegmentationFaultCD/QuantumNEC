#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/io.hpp>
#include <kernel/task/schedule/scheduler.hpp>
#include <kernel/task/task.hpp>
namespace Task {
auto initialize_task( std::uint64_t core ) -> void {
    auto main = new PCB {};

    main->name.append( Library::format( "init{}", core ) );
    main->PID = id_pool.get( );
    main->thread_group.push_back( PCB::Thread {} );
    auto &mthread = main->thread_group[ 0 ];
    using namespace Memory;
    using enum Memory::Page::Type;

    mthread.kernel_stack = (std::uint64_t)Page::allocator< P4Kib > {}.allocate( main->kernel_stack_size / Page::allocator< P4Kib >::__page_size__ );
    mthread.user_stack = (std::uint64_t)Page::allocator< P2Mib > {}.allocate( main->user_stack_size / Page::allocator< P2Mib >::__page_size__ );

    mthread.frame = (Interrupt::IDT::Frame *)physical_to_virtual( mthread.kernel_stack + PCB::kernel_stack_size - sizeof( Interrupt::IDT::Frame ) );

    std::construct_at( mthread.frame );

    main->running_thread = &mthread;
    main->page_table = nullptr;     // 为空说明默认使用内核页表

    main->schedule.reset( new Schedule );

    scheduler->first_initialize( *main );

    scheduler->get_current( ).core.running_task = std::move( *main );
    
}
PCB::PCB( std::string_view _name, std::uint64_t entry_offset, std::uint64_t text_physical, std::uint64_t text_segment_length ) :
    name { _name },
    page_table { new Memory::Paging::pml4t {} }, thread_group {}, PID { id_pool.get( ) }, schedule { new Schedule {} }, is_empty { false } {
    using namespace Memory;
    this->page_table->copy( *paging->kernel_page_table );
    using enum Memory::Page::Type;
    thread_group.push_back( Thread {} );
    auto &mthread = thread_group[ 0 ];
    mthread.kernel_stack = (std::uint64_t)Page::allocator< P4Kib > {}.allocate( this->kernel_stack_size / Page::allocator< P4Kib >::__page_size__ );
    mthread.user_stack = (std::uint64_t)Page::allocator< P2Mib > {}.allocate( this->user_stack_size / Page::allocator< P2Mib >::__page_size__ );
    mthread.frame = (Interrupt::IDT::Frame *)physical_to_virtual( mthread.kernel_stack + this->kernel_stack_size - sizeof( Interrupt::IDT::Frame ) );     // 内核栈栈底
    std::construct_at( mthread.frame );
    mthread.frame->cs = GDT::SELECTOR_CODE64_USER;
    mthread.frame->ss = GDT::SELECTOR_DATA64_USER;
    mthread.frame->regs.ds = GDT::SELECTOR_DATA64_USER;
    mthread.frame->regs.es = GDT::SELECTOR_DATA64_USER;
    mthread.frame->regs.fs = GDT::SELECTOR_DATA64_USER;
    mthread.frame->regs.gs = GDT::SELECTOR_DATA64_USER;

    this->page_table->map( (std::uint64_t)text_physical,
                           this->TEXT_SEGMENT,
                           ( text_segment_length + ( 4_KB - 1 ) ) / 4_KB,
                           this->page_table->PAGE_PRESENT | this->page_table->PAGE_RW_W | this->page_table->PAGE_US_U,
                           Page::Type::P4Kib );

    mthread.frame->rip = (void *)( TEXT_SEGMENT + entry_offset );

    this->page_table->map( mthread.user_stack,
                           this->USER_STACK_TOP - this->user_stack_size,
                           this->user_stack_size / Page::allocator< P2Mib >::__page_size__,
                           this->page_table->PAGE_PRESENT | this->page_table->PAGE_RW_W | this->page_table->PAGE_US_U,
                           Page::Type::P2Mib );

    mthread.frame->rsp = USER_STACK_TOP - 1;
    mthread.frame->rflags.IOPL = 0;
    mthread.frame->rflags.MBS = 1;
    mthread.frame->rflags.IF = 1;

    this->running_thread = &mthread;
}
auto PCB::save_context( Interrupt::IDT::Frame *frame ) -> PCB & {
    *this->running_thread->frame = *frame;
    return *this;
}
auto PCB::activate( void ) -> void {
    Memory::gdt->get_tss( this->schedule->cpu ).set_kstack( (std::uint64_t)Memory::physical_to_virtual( this->running_thread->kernel_stack + this->kernel_stack_size ) );

    scheduler->get_current( ).kgsbase = Memory::gdt->get_tss( this->schedule->cpu ).get_kstack( );
    Driver::IO::wrmsr( Memory::gdt->KERNEL_GS_BASE, (std::uint64_t)&scheduler->get_current( ).kgsbase );
    if ( ( (bool)this->page_table ) ) {
        this->page_table->activate( );
    }
    else {
        Memory::paging->kernel_page_table->activate( );
    }
}
PCB::PCB( PCB &&p ) :
    name { std::move( p.name ) }, PID { p.PID }, page_table { std::move( p.page_table ) }, running_thread { p.running_thread }, thread_group { std::move( p.thread_group ) }, schedule { std::move( p.schedule ) }, is_empty { false } {
    p.PID = -1;
    p.running_thread = nullptr;
    p.is_empty = true;
}
auto PCB::operator=( PCB &&p ) -> PCB & {
    this->name.clear( );
    this->name.append( p.name );
    this->PID = p.PID;
    this->page_table = std::move( p.page_table );
    this->running_thread = p.running_thread;
    this->thread_group = std::move( p.thread_group );
    this->schedule = std::move( p.schedule );
    this->is_empty = false;

    p.name.clear( );
    p.PID = -1;
    p.running_thread = nullptr;
    p.is_empty = true;
    return *this;
}

}     // namespace Task