#pragma once
#include <kernel/task/general/pcb/pcb.hpp>
#include <lib/Uefi.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
class __Scheduler__ {
public:
    enum class ErrorCode {
        ALL_QUEUE_ARE_EMPTY,
        CAN_NOT_INSERT_TASK,
        NO_TASK_CAN_SCHEDULER
    };
    explicit __Scheduler__( void ) = default;
    ~__Scheduler__( void )         = default;

public:
    auto insert( this auto &&self, PCB *pcb ) -> PCB * {
        return self.__insert__( pcb );
    }
    auto sleep( this auto &&self, uint64_t ticks ) -> std::expected< PCB *, ErrorCode > {
        return self.__sleep__( ticks );
    }
    auto wake_up( this auto &&self, PCB *pcb ) noexcept -> PCB * {
        return self.__wake_up__( pcb );
    }
    auto pick_next( this auto &&self ) -> std::expected< PCB *, ErrorCode > {
        return self.__pick_next__( );
    }
    auto schedule( this auto &&self ) -> std::expected< PCB *, ErrorCode > {
        return self.__schedule__( );
    }
    auto remove( this auto &&self ) -> std::expected< PCB *, ErrorCode > {
        return self.__remove__( );
    }
};
}     // namespace QuantumNEC::Kernel