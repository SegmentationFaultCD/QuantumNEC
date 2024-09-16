#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/task/process.hpp>
#include <Kernel/task/thread.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Task
    {
    public:
        explicit Task( VOID ) noexcept;
        virtual ~Task( VOID ) noexcept;
        template < typename PCB >
            requires std::is_compound_v< PCB >
        STATIC auto create( IN CONST char_t *_name, IN int64_t _priority, IN int64_t _flags, IN TaskFunction _entry, IN TaskArg _arg ) -> PCB * {
            if constexpr ( std::is_same_v< PCB, Process > ) {
                return new ( physical_to_virtual( Memory::page->malloc( 1, MemoryPageType::PAGE_2M ) ) ) Process { _name, _priority, _flags, _entry, _arg };
            }
            else if constexpr ( std::is_same_v< PCB, Thread > ) {
                return new ( physical_to_virtual( Memory::page->malloc( 1, MemoryPageType::PAGE_2M ) ) ) Thread { _name, _priority, _flags, _entry, _arg };
            }
            else {
                return NULL;
            }
        }

    public:
        inline STATIC Process *main_task;
        inline STATIC Process *idle_task;
        inline STATIC Process *ready_task;
    };
}
