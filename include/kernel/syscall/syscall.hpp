#pragma once
#include <array>
#include <cstdint>
namespace Kernel {
class Syscall {
public:
    struct Interface {
        std::uint64_t index;
        virtual auto handle( ) -> std::uint64_t = 0;
    };

    constexpr static auto NUMBER_OF_SYSCALL { 300 };

    auto initialize( void ) -> void;

    auto register_syscall( Interface *face ) -> void {
        if ( face->index < NUMBER_OF_SYSCALL ) {
            this->table[ face->index ] = face;
        }
    }

private:
    std::array< Interface *, NUMBER_OF_SYSCALL > table;
} inline syscall;
}     // namespace Kernel