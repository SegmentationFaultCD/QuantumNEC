#pragma once
#include <kernel/interrupt/x86_64/entry/entry.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class SymmetricMultiprocessingSwitchCPUEntry : public InterruptEntry {
public:
    explicit SymmetricMultiprocessingSwitchCPUEntry( void ) noexcept = default;
    virtual ~SymmetricMultiprocessingSwitchCPUEntry( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override;
};
}     // namespace QuantumNEC::Kernel::x86_64
