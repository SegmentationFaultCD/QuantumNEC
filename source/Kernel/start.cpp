#include "Kernel/Task/process.hpp"
#include <Arch/Arch.hpp>
#include <Kernel/kernel.hpp>
#include <Kernel/memory.hpp>
#include <Kernel/task.hpp>
#include <Lib/IO/Stream/iostream>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace QuantumNEC::Lib::Types;

//////////////////////////////////////分割线///////////////////////////////////////////////
/////////////////////////////////////以下为测试/////////////////////////////////////////////

Lib::Types::int64_t ProcC( Lib::Types::uint64_t code ) {
    Lib::IO::sout << "Process C: " << code << '\n';

    while ( true )
        ;
    return 0;
}
Lib::Types::int64_t ProcD( Lib::Types::uint64_t code ) {
    Lib::IO::sout << "Process D: " << code << "\n";
    while ( true )
        ;

    return 0;
}

Lib::Types::int64_t ProcE( Lib::Types::uint64_t code ) {
    // int i { };
    // while ( true ) {
    //     Lib::IO::sout << "E:" << ++i << '\n';

    // }

    Lib::IO::sout << "Process E: " << code << "\n";
    ASM( "INT $0x80\n\t" ::"a"( 3 ), "D"( 3 ) );
    // Kernel::Message message { };
    // while ( true ) {
    //     message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_RECEIVE, Kernel::ANY );
    //     Lib::IO::sout << "E:\n";
    //     message.send_receive( Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction::MESSAGE_SEND, 4 );
    // }

    while ( true )
        ;
    return 0;
}
Lib::Types::int64_t ProcF( Lib::Types::uint64_t code ) {
    Lib::IO::sout << "Process F: " << code << "\n";
    // int i { };
    // while ( true ) {
    //     Lib::IO::sout << "F:" << ++i << '\n';
    // }
    while ( true )
        ;
    return 0;
}

//////////////////////////////////////分割线///////////////////////////////////////////////

_C_LINK auto micro_kernel_entry( IN Ptr< BootConfig > config ) -> SystemStatus {
    SystemStatus Status { SYSTEM_SUCCESS };
    Architecture::ArchitectureManager< TARGET_ARCH > architecture { config };     // 系统架构初始化
    Kernel::Memory memory { config };                                             // 内存管理初始化
    Kernel::Task task { config };                                                 // 进程管理初始化
    Lib::IO::sout << "Test 1 : Memory allocate-------------------------------\n";
    char buf[] { "hello world\0" };
    auto w { new char[ 12 ] };
    Lib::STL::strcpy( w, buf );
    Lib::IO::sout << w << " " << (void *)w << "\n";
    delete[] w;

    auto a = new char[ 12 ];
    Lib::STL::strcpy( a, buf );
    Lib::IO::sout << a << " " << (void *)a << "\n";

    auto b = new char[ 12 ];

    Lib::STL::strcpy( b, buf );
    Lib::IO::sout << b << " " << (void *)b << "\n";
    delete[] b;

    auto c = new char[ 0x12 ];
    Lib::STL::strcpy( c, buf );
    Lib::IO::sout << c << " " << (void *)c << "\n";

    auto d = new char[ 0x123 ];
    Lib::STL::strcpy( d, buf );
    Lib::IO::sout << d << " " << (void *)d << "\n";
    delete[] c;

    auto f = new char[ 1 ];
    Lib::STL::strcpy( f, buf );
    Lib::IO::sout << f << " " << (void *)f << "\n";
    delete[] f;

    auto g = new char[ 0x200000 ];
    Lib::STL::strcpy( g, buf );
    Lib::IO::sout << g << ' ' << (void *)g << '\n';
    {
        Lib::STL::string str { "Hell oworl" };
        str += " CAO\n";
        Lib::IO::sout << str.c_str( );
    }

    Lib::IO::sout << "Test 2 : Make 2 processes-------------------------------\n";
    /* 开启中断 */

    task.create( "Process C", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_USER_PROCESS, ProcC, 0 );
    task.create( "Process D", 31, Kernel::TASK_FLAG_FPU_UNUSED | Kernel::TASK_FLAG_USER_PROCESS, ProcD, 0 );
    // Kernel::Task::create( ProcE, 20, Kernel::Task::TaskType::PF_KERNEL_PROCESS, "Process E", 100, static_cast< Lib::Types::int64_t >( Kernel::TaskManagement::TaskFlags::FPU_UNUSED ) );
    // Kernel::Task::create( ThreadA, 20, Kernel::Task::TaskType::PF_KERNEL_THREAD, "Thread A", 1000, static_cast< Lib::Types::int64_t >( Kernel::TaskManagement::TaskFlags::FPU_UNUSED ) );
    // Kernel::Task::create( ThreadB, 20, Kernel::Task::TaskType::PF_KERNEL_THREAD, "Thread B", 31, static_cast< Lib::Types::int64_t >( Kernel::TaskManagement::TaskFlags::FPU_UNUSED ) );
    // Kernel::Task::create( ProcF, 20, Kernel::Task::TaskType::PF_KERNEL_PROCESS, "Process F", 100, static_cast< Lib::Types::int64_t >( Kernel::TaskManagement::TaskFlags::FPU_UNUSED ) );
    // Kernel::Task::create( ThreadC, 20, Kernel::Task::TaskType::PF_KERNEL_THREAD, "Thread C", 100, static_cast< Lib::Types::int64_t >( Kernel::TaskManagement::TaskFlags::FPU_UNUSED ) );

    task.main_task->block( Kernel::TaskStatus::BLOCKED );     // 锁死主进程，相当于直接抛弃

    Architecture::ArchitectureManager< TARGET_ARCH >::enable_interrupt( );

    while ( true )
        ;

    return Status;
}
