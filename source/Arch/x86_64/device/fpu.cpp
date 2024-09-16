#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/task.hpp>
#include <Kernel/memory.hpp>
#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Architecture {
    inline STATIC Lib::Types::Ptr< VOID > last_fpu_task { };
    FPU::FPU( VOID ) noexcept {
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Initialize the FPU Management" << Lib::IO::endl;
        // Lib::Types::BOOL exist { this->check_fpu( ) };
        // if ( exist ) {
        //     // 设置 CR0 寄存器
        //     CPU::CPUManagement::write_cr0( CPU::CPUManagement::read_cr0( ) | static_cast< Lib::Types::int64_t >( CR0_COMMAND::EM ) | static_cast< Lib::Types::int64_t >( CR0_COMMAND::TS ) | static_cast< Lib::Types::int64_t >( CR0_COMMAND::NE ) );
        // }
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the FPU Management" << Lib::IO::endl;
    }
    auto FPU::FPUFrame::flush_fpu( VOID )->VOID {
        ASM( "fnclex \n\t"
             "fninit \n\t" );
    }
    auto FPU::FPUFrame::check_fpu( VOID )->Lib::Types::BOOL {
        Lib::Types::uint32_t code { 114514 };
        ASM(
            "MOVQ %%CR0,%%RAX \n\t"
            "ANDB $0xf3,%%AL \n\t"
            "ORB  $0x02,%%AL \n\t"
            "MOVQ %%RAX,%%CR0 \n\t"
            "MOVQ %%CR4,%%RAX \n\t"
            "ORW  $3 << 9,%%AX \n\t"
            "MOVQ %%RAX,%%CR4 \n\t"
            "fninit \n\t"
            "fnstsw %0 \n\t"
            :
            : "m"( code )
            : "rax" );
        return !code;
    }
    auto FPU::FPUFrame::enable_fpu( IN Lib::Types::Ptr< VOID > task )->VOID {
        auto cr0 = CPUs::read_cr0( );
        cr0.EM = 1;
        cr0.TS = 1;
        CPUs::write_cr0( cr0 );
        auto _last_fpu_task { reinterpret_cast< Kernel::Process * >( last_fpu_task ) }, _task { reinterpret_cast< Kernel::Process * >( task ) };
        if ( _last_fpu_task == task ) {
            // 没有变化，不需要恢复浮点环境
            return;
        }
        if ( _last_fpu_task && _last_fpu_task->flags & Kernel::TASK_FLAG_FPU_ENABLE ) {
            // 存在使用过浮点处理单元的进程，需要保存浮点环境
            if ( _last_fpu_task->fpu_frame ) {
                while ( true )
                    ;
            }

            ASM( "fnsave (%0)\n" ::"r"( _last_fpu_task->fpu_frame ) );
            _last_fpu_task->flags &= ~( Kernel::TASK_FLAG_FPU_ENABLE );
        }

        _last_fpu_task = _task;

        // 如果 fpu 不为空，则恢复浮点环境
        if ( _task->fpu_frame ) {
            ASM( "frstor (%0) \n" ::"r"( _task->fpu_frame ) );
        }
        else {
            // 否则，初始化浮点环境
            flush_fpu( );
            _task->flags |= ( Kernel::TASK_FLAG_FPU_ENABLE | Kernel::TASK_FLAG_FPU_USED );
        }
    }

    auto FPU::FPUFrame::disable_fpu( IN Lib::Types::Ptr< VOID > )->VOID {
        auto cr0 = CPUs::read_cr0( );
        cr0.EM = 1;
        cr0.TS = 1;
        CPUs::write_cr0( cr0 );
    }
}