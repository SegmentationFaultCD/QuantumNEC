#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
#include <modules/modules.hpp>
using namespace std;
namespace {

__attribute__( ( used, section( ".requests" ) ) ) volatile LIMINE_BASE_REVISION( 2 );

}

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

namespace {

__attribute__( ( used, section( ".requests" ) ) ) volatile limine_framebuffer_request framebuffer_request = {
    .id       = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_memmap_request memmap_request = {
    .id       = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_rsdp_request acpi_request = {
    .id       = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_hhdm_request hhdm_request = {
    .id       = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_smp_request smp_request = {
    .id       = LIMINE_SMP_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_paging_mode_request paging_mode_request = {
    .id       = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__( ( used, section( ".requests" ) ) ) volatile limine_module_request modules_request = {
    .id       = LIMINE_MODULE_REQUEST,
    .revision = 0,
    .response = nullptr
};
}     // namespace

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__( ( used, section( ".requests_start_marker" ) ) ) volatile LIMINE_REQUESTS_START_MARKER;

__attribute__( ( used, section( ".requests_end_marker" ) ) ) volatile LIMINE_REQUESTS_END_MARKER;

}     // namespace
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
SpinLock _lock { };
int64_t  ProcC( uint64_t ) {
    // Architecture::Message message { };
    // Architecture::Syscall::SyscallStatus a;

    // while ( true ) {
    //     send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_RECEIVE, Architecture::ANY, message );
    //     println< ostream::HeadLevel::DEBUG >( "Process Csfdsfsfsfsf" );
    //     send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 3, message );
    // }
    println< ostream::HeadLevel::DEBUG >( "Process Csfdsfsfsfsf" );
    while ( true );

    return 0;
}
int64_t ProcD( uint64_t ) {
    // Architecture::Message message { };
    // Architecture::Syscall::SyscallStatus a;
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    // println< ostream::HeadLevel::DEBUG >( "Process D" );
    // send_receive( a, Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND_RECEIVE, 2, message );
    println< ostream::HeadLevel::DEBUG >( "Process D" );
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
    println< ostream::HeadLevel::DEBUG >( "HELLOWORLD" );
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
Kernel::HeapAllocater a { };
Kernel::HeapCollector c { };
extern "C" {
void *malloc( size_t size ) {
    return a.allocate( size );
};
void free( void *s ) {
    c.free( s );
};
}
#define TERMINAL_EMBEDDED_FONT
#include <extern/os_terminal.h>
_C_LINK auto micro_kernel_entry( VOID ) -> VOID {
    Kernel::__config.graphics_data = *framebuffer_request.response->framebuffers[ 0 ];
    Kernel::__config.memory_map    = *memmap_request.response;
    Kernel::__config.acpi_table    = *acpi_request.response;
    Kernel::__config.hhdm          = *hhdm_request.response;
    Kernel::__config.smp           = *smp_request.response;
    Kernel::__config.paging_mode   = *paging_mode_request.response;
    Kernel::__config.modules       = *modules_request.response;

    Kernel::Graphics  grap { };
    Kernel::Acpi      acp { };
    Kernel::Interrupt intr { };
    Kernel::Memory    mem { };
    Kernel::Sound     soun { };
    Kernel::Time      tim { };
    Kernel::Syscall   sysc { };
    Kernel::CPU       cpu { };
    // Kernel::CPU       cpu { };
    // Kernel::Task task { };

    println< ostream::HeadLevel::DEBUG >( "start allocate" );

    println< ostream::HeadLevel::DEBUG >( "finish!" );
    println< ostream::HeadLevel::DEBUG >( "Begin test malloc and free" );
    char *a = (char *)malloc( 0x100 );
    char *b = (char *)malloc( 0x200 );
    char *c = (char *)malloc( 0x4000 );
    println< ostream::HeadLevel::DEBUG >( "A: {:x}", (void *)a );
    println< ostream::HeadLevel::DEBUG >( "B: {:x}", (void *)b );
    println< ostream::HeadLevel::DEBUG >( "C: {:x}", (void *)c );
    // bit pattern
    for ( size_t i = 0; i < 0x100; i++ ) {
        a[ i ] = 0x01;
    }
    // test if the memory is valid
    for ( size_t i = 0; i < 0x100; i++ ) {
        if ( a[ i ] != 0x01 ) {
            println< ostream::HeadLevel::DEBUG >( "Error at {:x}", (void *)a );
        }
    }
    println< ostream::HeadLevel::DEBUG >( "Free A" );

    for ( size_t i = 0; i < 0x200; i++ ) {
        b[ i ] = 0x02;
    }
    for ( size_t i = 0; i < 0x200; i++ ) {
        if ( b[ i ] != 0x02 ) {
            println< ostream::HeadLevel::DEBUG >( "Error at {:x}", (void *)b );
        }
    }
    println< ostream::HeadLevel::DEBUG >( "Free B" );

    for ( size_t i = 0; i < 0x4000; i++ ) {
        c[ i ] = 0x03;
    }
    for ( size_t i = 0; i < 0x4000; i++ ) {
        if ( c[ i ] != 0x03 ) {
            println< ostream::HeadLevel::DEBUG >( "Error at {:x}", (void *)c );
        }
    }
    println< ostream::HeadLevel::DEBUG >( "Free C" );

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
    }
}
