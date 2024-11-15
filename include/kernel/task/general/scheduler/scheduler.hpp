#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
#include <libcxx/expected.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class Scheduler {
    public:
        enum class ErrorCode {
            ALL_QUEUE_ARE_EMPTY,
            CAN_NOT_INSERT_TASK,
        };
        explicit Scheduler( VOID ) = default;
        ~Scheduler( VOID )         = default;

    public:
        auto sleep( this auto &&self, uint64_t ticks ) -> std::expected< PCB *, ErrorCode > {
            return self.__sleep__( ticks );
        }
        auto wake_up( this auto &&self, PCB *pcb ) -> std::expected< PCB *, ErrorCode > {
            return self.__wake_up__( pcb );
        }
        auto pick_next( this auto &&self ) -> std::expected< PCB *, ErrorCode > {
            return self.__pick_next__( );
        }
        auto schedule( this auto &&self ) -> std::expected< PCB *, ErrorCode > {
            return self.__schedule__( );
        }
    };
}