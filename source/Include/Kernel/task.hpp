#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Kernel/Task/process.hpp>
#include <Kernel/Task/thread.hpp>
#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Task
    {
    public:
        explicit Task( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;
        virtual ~Task( VOID ) noexcept;

        auto create( IN CONST Lib::Types::char_t *_name, IN Lib::Types::int64_t _priority, IN Lib::Types::int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) -> VOID * {
            auto pcb { Memory::page->malloc( 1, PageMemory::MemoryPageType::PAGE_2M ) };
            if ( ( _flags & TASK_FLAG_KERNEL_PROCESS ) || ( _flags & TASK_FLAG_USER_PROCESS ) ) {
                return (VOID *)new ( pcb ) Process { _name, _priority, _flags, _entry, _arg };
            }
            else if ( _flags & TASK_FLAG_THREAD ) {
                return (VOID *)new ( pcb ) Thread { _name, _priority, _flags, _entry, _arg };
            }
            else {
                Memory::page->free( pcb, 1, PageMemory::MemoryPageType::PAGE_2M );
                return NULL;
            }
        }

    public:
        inline STATIC Process *main_task;
        inline STATIC Process *idle_task;
        inline STATIC Process *ready_task;
    };
}
