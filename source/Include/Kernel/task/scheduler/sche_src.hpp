#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/task/pcb/pcb.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ScheduleSource
    {
    public:
        ScheduleSource( ) = default;
        virtual ~ScheduleSource( ) = default;

    public:
        virtual auto insert( IN PCB *pcb ) -> VOID = 0;
        virtual auto sleep( ) -> VOID = 0;
        virtual auto wake_up( ) -> VOID = 0;
        virtual auto schedule( ) -> VOID = 0;
    };
}