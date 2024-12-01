#pragma once
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class SystemcallEntry : public InterruptEntry {
    public:
        explicit SystemcallEntry( VOID ) noexcept = default;
        virtual ~SystemcallEntry( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override;
    };
}