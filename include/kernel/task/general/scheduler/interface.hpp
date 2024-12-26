#pragma once
#include <concepts>
#include <lib/Uefi.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
class SchedulerInterface;

template < typename Scheduler >
concept conform_to_scheduler_standard = std::derived_from< Scheduler, SchedulerInterface >     // must derived from base class __Scheduler__
                                        && std::constructible_from< Scheduler >                // must have a Default constructor
                                        && requires( Scheduler::Schedule schedule ) {          // must obey interface agreement
                                               { Scheduler { }.__insert__( schedule ) } -> std::same_as< typename Scheduler::Schedule * >;
                                               { Scheduler { }.__sleep__( ) } -> std::same_as< std::expected< typename Scheduler::Schedule *, typename Scheduler::ErrorCode > >;
                                               { Scheduler { }.__wake_up__( schedule ) } -> std::same_as< typename Scheduler::Schedule * >;
                                               { Scheduler { }.__pick_next__( ) } -> std::same_as< std::expected< typename Scheduler::Schedule *, typename Scheduler::ErrorCode > >;
                                               { Scheduler { }.__schedule__( ) } -> std::same_as< std::expected< typename Scheduler::Schedule *, typename Scheduler::ErrorCode > >;
                                               { Scheduler { }.__remove__( ) } -> std::same_as< std::expected< typename Scheduler::Schedule *, typename Scheduler::ErrorCode > >;
                                           };

class SchedulerInterface {
public:
    explicit SchedulerInterface( void ) = default;
    ~SchedulerInterface( void )         = default;

public:
    auto insert( this auto &&self, auto &schedule ) {
        return self.__insert__( schedule );
    }
    auto sleep( this auto &&self ) {
        return self.__sleep__( );
    }
    auto wake_up( this auto &&self, auto &schedule ) noexcept {
        return self.__wake_up__( schedule );
    }
    auto pick_next( this auto &&self ) {
        return self.__pick_next__( );
    }
    auto schedule( this auto &&self ) {
        return self.__schedule__( );
    }
    auto remove( this auto &&self ) {
        return self.__remove__( );
    }
};
}     // namespace QuantumNEC::Kernel