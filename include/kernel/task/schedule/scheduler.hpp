#pragma once
#include "./Muqss.hpp"
#include <kernel/task/task.hpp>
namespace Task {

template < typename T >
    requires requires {
        typename T::mCore;
    }
struct Scheduler : T {
    friend T;
    struct Core {
        T::mCore core;
        std::uint64_t kgsbase;
        std::uint64_t gsbase;
        explicit Core( ) :
            core { } {}
        explicit Core( Core &&c ) :
            kgsbase { c.kgsbase }, gsbase { c.gsbase },
            core { std::move( c.core ) } {}
        auto operator=( Core &&c ) -> Core & {
            kgsbase = c.kgsbase;
            c.kgsbase = 0;
            gsbase = c.gsbase;
            c.gsbase = 0;
            core = std::move( c.core );
            return *this;
        }
        ~Core( ) = default;
    };
    static auto initialize( void ) -> void {}
    auto get_current( ) -> Core &;

private:
    Library::RBTree< std::uint64_t, Core > running_queue;
};

inline Scheduler< Muqss > *scheduler;

}     // namespace Task