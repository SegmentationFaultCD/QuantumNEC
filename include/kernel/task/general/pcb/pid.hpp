#pragma once
#include <lib/Uefi.hpp>
#include <libcxx/bitset.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
constexpr auto PID_COUNT { 1024ul };
class PidPool {
public:
    explicit PidPool( void ) noexcept = default;

public:
    auto allocate( void ) -> uint64_t {
        return this->global_pid++;
    }

private:
    uint64_t global_pid { };
};
inline PidPool pid_pool { };
}     // namespace QuantumNEC::Kernel