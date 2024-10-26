#pragma once
#include <lib/Uefi.hpp>
#include <libcxx/bitset.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
PUBLIC inline constexpr CONST auto PID_COUNT { 1024ul };
class PidPool
{
public:
    explicit PidPool( VOID ) noexcept = default;

public:
    auto allocate( VOID ) -> uint64_t {
        return this->global_pid++;
    }

private:
    uint64_t global_pid { };
};
PUBLIC inline PidPool pid_pool { };
}     // namespace QuantumNEC::Kernel