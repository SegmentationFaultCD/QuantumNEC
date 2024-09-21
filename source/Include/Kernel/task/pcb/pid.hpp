#pragma once
#include <Lib/Uefi.hpp>
#include <Libcxx/bitset.hpp>
#include <Libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
PUBLIC inline constexpr CONST auto PID_COUNT { 1024ul };
class PidPool
{
public:
    explicit PidPool( VOID ) noexcept = default;

public:
    auto allocate( VOID ) -> std::expected< uint64_t, int32_t > {
        auto index = this->bitmap.allocate( 1 );
        if ( index.has_value( ) ) {
            return index.value( );
        }
        else {
            return std::unexpected { -1 };
        }
    }
    auto free( IN uint64_t pid ) {
        if ( pid >= PID_COUNT ) {
            return;
        }
        this->bitmap.free( pid );
    }

private:
    std::bitset< PID_COUNT > bitmap { };
};
PUBLIC inline PidPool pid_pool { };
}     // namespace QuantumNEC::Kernel