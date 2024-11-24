#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/kheap/kheap_collector.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_collector.hpp>

#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/process/process_walker.hpp>
#include <kernel/task/task.hpp>
#include <modules/modules.hpp>

using namespace std;
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
#define TERMINAL_EMBEDDED_FONT
#include <extern/os_terminal.h>
SpinLock _lock { };
int64_t  ProcC( VOID ) {
    // Architecture::Message message { };
    // Architecture::Syscall::SyscallStatus a;

    // while ( true ) {
    //     send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_RECEIVE, Architecture::ANY, message );
    //     println< ostream::HeadLevel::DEBUG >( "Process Csfdsfsfsfsf" );
    //     send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 3, message );
    // }
    println< print_level::DEBUG >( "Process Csfdsfsfsfsf" );
    while ( true );

    return 0;
}
int64_t ProcD( uint64_t ) {
    // Architecture::Message message { };
    // Architecture::Syscall::SyscallStatus a;
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    // println< ostream::HeadLevel::DEBUG >( "Process D" );
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    println< print_level::DEBUG >( "Process D" );
    while ( true );
    return 0;
}
int64_t ProcE( uint64_t ) {
    // Kernel::Message message { };
    // Architecture::Syscall::SyscallStatus a;
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    // Kernel::Message message { };
    // // F( &message, 2, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND );
    // message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 2 );
    // Lib::IO::sout << "D:\n";
    // // F( &message, 2, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND );
    // message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 2 );
    // Lib::IO::sout << "D:\n";

    while ( true );
    return 0;
}
int64_t ThreadA( uint64_t ) {
    // int i { };
    // while ( true ) {
    //     Lib::IO::sout << "E:" << ++i << '\n';
    // }
    // Lib::println< Lib::ostream::HeadLevel::DEBUG >( "HELLOWORLD" );
    // Kernel::Message message { };
    // while ( true ) {
    //     message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_RECEIVE, Kernel::ANY );
    //     Lib::IO::sout << "E:\n";
    //     message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 4 );
    // }

    while ( true ) {
    }
    return 0;
}

int64_t ThreadB( uint64_t ) {
    // int i { };
    // while ( true ) {
    //     Lib::IO::sout << "E:" << ++i << '\n';
    // }
    println< print_level::DEBUG >( "HELLOWORLD" );
    // Kernel::Message message { };
    // while ( true ) {
    //     message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_RECEIVE, Kernel::ANY );
    //     Lib::IO::sout << "E:\n";
    //     message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 4 );
    // }
    while ( true ) {
    }
    return 0;
}
SpinLock lock { };
using namespace QuantumNEC;
auto malloc( size_t size ) -> void * {
    return Kernel::KHeapAllocater { }.allocate( size );
};
auto free( void *addr ) -> void {
    Kernel::KHeapCollector { }.free( addr );
};
#include <lib/rbtree.hpp>
auto micro_kernel_entry( IN BootConfig &config ) -> VOID {
    Kernel::__config = config;
    // Kernel::__config.graphics_data = *framebuffer_request.response->framebuffers[ 0 ];
    // Kernel::__config.memory_map    = *memmap_request.response;
    // Kernel::__config.acpi_table    = *acpi_request.response;
    // Kernel::__config.hhdm          = *hhdm_request.response;
    // Kernel::__config.smp           = *smp_request.response;
    // Kernel::__config.paging_mode   = *paging_mode_request.response;
    // Kernel::__config.modules       = *modules_request.response;

    Kernel::Output    outpt { };
    Kernel::Acpi      acp { };
    Kernel::Interrupt intr { };
    Kernel::Memory    mem { };
    Kernel::CPU       cpu { };
    Kernel::Sound     soun { };
    Kernel::Time      tim { };
    Kernel::Syscall   sysc { };

    Modules::Module        mod { };
    Kernel::Task           task { };
    Kernel::ProcessCreater cre;
    cre.create( "Proc1", Kernel::Scheduler::IDLEPRIO, (VOID *)ProcC, Kernel::PCB::Type::KERNEL_PROCESS );

    Kernel::Interrupt::enable_interrupt( );
    // STATIC TerminalDisplay s;
    // s.address   = (QuantumNEC::uint8_t *)Kernel::__config.graphics_data.address;
    // s.height    = Kernel::__config.graphics_data.height;
    // s.width     = Kernel::__config.graphics_data.width;
    // auto result = terminal_init(
    //     &s,
    //     12.0,
    //     malloc,
    //     free,
    //     NULL );
    // auto                   result = cre.create( "NAME!", 1, (VOID *)ProcC, Kernel::PCB::Type::KERNEL_PROCESS );
    // std::println< std::ostream::HeadLevel::DEBUG >( "{}", result.value( )->virtual_deadline );
    // println< ostream::HeadLevel::DEBUG >( "start allocate" );

    // println< ostream::HeadLevel::DEBUG >( "finish!" );
    // println< ostream::HeadLevel::DEBUG >( "Begin test malloc and free" );
    // char *a = (char *)Kernel::KHeapAllocater { }.allocate( 0x100 );
    // char *b = (char *)Kernel::KHeapAllocater { }.allocate( 0x200 );
    // char *c = (char *)Kernel::KHeapAllocater { }.allocate( 0x4000 );
    // println< ostream::HeadLevel::DEBUG >( "A: {:x}", (void *)a );
    // println< ostream::HeadLevel::DEBUG >( "B: {:x}", (void *)b );
    // println< ostream::HeadLevel::DEBUG >( "C: {:x}", (void *)c );
    // // bit pattern
    // for ( size_t i = 0; i < 0x100; i++ ) {
    //     a[ i ] = 0x01;
    // }
    // // test if the memory is valid
    // for ( size_t i = 0; i < 0x100; i++ ) {
    //     if ( a[ i ] != 0x01 ) {
    //         println< ostream::HeadLevel::DEBUG >( "Error at {:x}", (void *)a );
    //     }
    // }
    // println< ostream::HeadLevel::DEBUG >( "Free A" );

    // for ( size_t i = 0; i < 0x200; i++ ) {
    //     b[ i ] = 0x02;
    // }
    // for ( size_t i = 0; i < 0x200; i++ ) {
    //     if ( b[ i ] != 0x02 ) {
    //         println< ostream::HeadLevel::DEBUG >( "Error at {:x}", (void *)b );
    //     }
    // }
    // println< ostream::HeadLevel::DEBUG >( "Free B" );

    // for ( size_t i = 0; i < 0x4000; i++ ) {
    //     c[ i ] = 0x03;
    // }
    // for ( size_t i = 0; i < 0x4000; i++ ) {
    //     if ( c[ i ] != 0x03 ) {
    //         println< ostream::HeadLevel::DEBUG >( "Error at {:x}", (void *)c );
    //     }
    // }

    // Kernel::Interrupt::enable_interrupt( );
    // for ( auto i = 0; i < 2000; ++i ) {
    //     auto s = Kernel::PageAllocater { }.allocate< Kernel::MemoryPageType::PAGE_2M >( 1 );
    //     println< ostream::HeadLevel::DEBUG >( "{:x} {}", s, i );
    // }
    // println< print_level::DEBUG >( "Free C" );
    // auto g = Kernel::PageAllocater { }.allocate< Kernel::MemoryPageType::PAGE_2M >( 12 );
    // println< print_level::DEBUG >( "{:x}", g );
    // auto h = Kernel::PageAllocater { }.allocate< Kernel::MemoryPageType::PAGE_2M >( 100 );
    // println< print_level::DEBUG >( "{:x}", h );
    // auto s = Kernel::PageAllocater { }.allocate< Kernel::MemoryPageType::PAGE_2M >( 1523 );
    // println< print_level::DEBUG >( "{:x}", s );
    // Kernel::PageCollector { }.free< Kernel::PAGE_2M >( s, 1523 );
    // auto r = Kernel::PageAllocater { }.allocate< Kernel::MemoryPageType::PAGE_2M >( 2048 );
    // println< print_level::DEBUG >( "{:x}", r );

    // task.create< Kernel::Process >( "Process C", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_KERNEL_PROCESS, ProcC, 0 );
    // task.create< Kernel::Process >( "Process D", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_KERNEL_PROCESS, ProcD, 0 );

    // // task.create( "Thread A", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_THREAD, ThreadA, 0 );
    // // task.create( "Thread B", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_THREAD, ThreadB, 0 );

    // // task.create( "Process D", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_KERNEL_PROCESS, ProcD, 0 );
    // // task.create( "Process E", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_KERNEL_PROCESS, ProcE, 0 );
    // // task.create( "Process E", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_KERNEL_PROCESS, ProcE, 0 );

    // Architecture::ArchitectureManager< TARGET_ARCH >::enable_interrupt( );

    // task.main_task->block( Kernel::TaskStatus::BLOCKED );

    // 锁死主进程，相当于直接抛弃

    // Architecture::Apic::IntCmdRet icr { };

    // icr.vector = 0xc8;
    // icr.destination.x2apic_destination = 1;
    // icr.deliver_mode = Architecture::APIC_ICR_IOAPIC_FIXED;
    // architecture.wrmsr( 0x830, icr );
    while ( true ) {
        Kernel::CPU::hlt( );
    }
}
