#include <kernel/print.hpp>
#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/memory.hpp>
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
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_rsdp_request acpi_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
    .response = nullptr
};
__attribute__( ( used, section( ".requests" ) ) ) volatile limine_paging_mode_request paging_mode_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__( ( used, section( ".requests" ) ) ) volatile limine_module_request modules_request = {
    .id = LIMINE_MODULE_REQUEST,
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
int64_t ProcC( uint64_t ) {
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

_C_LINK auto micro_kernel_entry( VOID ) -> VOID {
    QuantumNEC::Kernel::__config.graphics_data = *framebuffer_request.response->framebuffers[ 0 ];
    QuantumNEC::Kernel::__config.memory_map = *memmap_request.response;
    QuantumNEC::Kernel::__config.acpi_table = *acpi_request.response;
    QuantumNEC::Kernel::__config.hhdm = *hhdm_request.response;
    QuantumNEC::Kernel::__config.smp = *smp_request.response;
    QuantumNEC::Kernel::__config.paging_mode = *paging_mode_request.response;
    QuantumNEC::Kernel::__config.modules = *modules_request.response;

    Kernel::Graphics grap { };
    Kernel::Acpi acp { };
    Kernel::Interrupt intr { };
    Kernel::Memory mem { };     // 内存管理初始化
    Kernel::Sound soun { };
    Kernel::Time tim { };
    // Kernel::Task task { };                                                 // 进程管理初始化
    Kernel::Syscall sysc { };
    Kernel::CPU cpu { };
    Modules::Module modules { };
    println< ostream::HeadLevel::DEBUG >( "Test 1 : Memory allocate--------------------------------" );

    Kernel::PageAllocater allocater { };
    Kernel::PageCollector collector { };
    using namespace QuantumNEC::Kernel;
    auto s = allocater.allocate< MemoryPageType::PAGE_2M >( 1028 );
    println< ostream::HeadLevel::DEBUG >( "{}", s );
    collector.free< MemoryPageType::PAGE_2M >( s, 1028 );

    s = allocater.allocate< MemoryPageType::PAGE_2M >( 1028 );
    println< ostream::HeadLevel::DEBUG >( "{}", s );
    collector.free< MemoryPageType::PAGE_2M >( s, 1028 );

    // char buf[] { "hello world\0" };
    // auto w { new char[ 12 ] };
    // strcpy( w, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", w, (void *)w );
    // delete[] w;
    // auto a = new char[ 12 ];
    // strcpy( a, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", a, (void *)a );
    // auto b = new char[ 12 ];
    // strcpy( b, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", b, (void *)b );
    // delete[] b;
    // auto c = new char[ 0x12 ];
    // strcpy( c, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", c, (void *)c );
    // auto d = new char[ 0x123 ];
    // strcpy( d, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", d, (void *)d );
    // delete[] c;
    // auto f = new char[ 1 ];
    // strcpy( f, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", f, (void *)f );
    // delete[] f;
    // auto g = new char[ 0x200001 ];
    // strcpy( g, buf );
    // println< ostream::HeadLevel::DEBUG >( "{} {:x}", g, (void *)g );

    println< ostream::HeadLevel::DEBUG >( "Test 2 : Make 2 processes-------------------------------" );

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
