#pragma once
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class InterruptEntry {
protected:
    using Frame = InterruptDescriptorTable::InterruptFrame;

private:
    virtual auto name( void ) noexcept -> void               = 0;
    virtual auto handler( Frame *frame ) noexcept -> Frame * = 0;
    virtual auto error_code( uint64_t error_code ) noexcept -> void;

protected:
    auto registers( Frame *frame ) noexcept -> void;
    auto task( void ) noexcept -> void;
    auto cpu( void ) noexcept -> void;

public:
    virtual auto do_register( void ) -> void = 0;

public:
    explicit InterruptEntry( void ) noexcept = default;
    virtual ~InterruptEntry( void ) noexcept = default;

public:
    auto do_IRQ( Frame *frame ) -> Frame * {
        this->name( );
        this->error_code( frame->error_code );
        return this->handler( frame );
    }

private:
    struct ErrorCode {
        uint32_t external : 1;
        uint32_t tbl : 2;
        uint32_t index : 13;
        uint32_t reserved : 16;
        ErrorCode( uint64_t error_code ) {
            *reinterpret_cast< uint64_t * >( this ) = error_code;
        }
    };
};

}     // namespace QuantumNEC::Kernel::x86_64