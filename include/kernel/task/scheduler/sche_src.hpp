#pragma once
#include <lib/Uefi.hpp>
#include <kernel/task/manager/pcb.hpp>
#include <libcxx/expected.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ScheduleSource
    {
    public:
        enum class ErrorCode {
            ALL_QUEUE_ARE_EMPTY,
            CAN_NOT_INSERT_TASK,
        };
        ScheduleSource( ) = default;
        virtual ~ScheduleSource( ) = default;

    public:
        virtual auto sleep( ) -> std::expected< PCB *, ErrorCode > = 0;
        virtual auto wake_up( PCB * ) -> std::expected< PCB *, ErrorCode > = 0;
        virtual auto pick_next( ) -> std::expected< PCB *, ErrorCode > = 0;
        virtual auto schedule( ) -> std::expected< PCB *, ErrorCode > = 0;
    };
}