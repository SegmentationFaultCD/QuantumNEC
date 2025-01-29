
#include <kernel/cpu/cpu.hpp>
#include <kernel/driver/driver.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
#define TERMINAL_EMBEDDED_FONT
#include <libos-terminal/os_terminal.h>

#include <modules/modules.hpp>

using namespace std;
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;

using namespace QuantumNEC;
// extern "C" auto m( std::size_t size ) -> void * {
//     return Kernel::KHeapAllocator< void > { }.allocate( size );
// }
// extern "C" auto f( void *address ) -> void {
//     return Kernel::KHeapAllocator< void > { }.deallocate( address, 1 );
// }
// extern "C" auto s( const char *str ) -> void {
//     return;
// }
#include <kernel/task/process/process.hpp>

auto micro_kernel_entry( IN BootConfig &config ) -> void {
    Kernel::__config = config;

    Kernel::Output outpt { };

    Kernel::Acpi acp { };

    Kernel::Interrupt intr { };
    Kernel::Memory    mem { };
    // TerminalDisplay   display {
    //       .width   = config.graphics_data.width,
    //       .height  = config.graphics_data.height,
    //       .address = (uint32_t *)config.graphics_data.address
    // };
    Kernel::Task task { };
    Kernel::CPU  cpu { };

    // auto result = terminal_init( &display, 10.0f, m, f, nullptr );
    // std::println( "{}", (int)result );
    // while ( true );

    Kernel::Sound   soun { };
    Kernel::Time    tim { };
    Kernel::Syscall sysc { };

    Modules::Module mod { };

    Kernel::Interrupt::enable_interrupt( );

    while ( true ) {
    }
}
