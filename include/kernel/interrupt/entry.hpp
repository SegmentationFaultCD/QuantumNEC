#pragma once
#include <kernel/interrupt/idt.hpp>
#include <string_view>
namespace Interrupt {
extern "C" auto do_IRQ( IDT::Frame *frame ) -> IDT::Frame *;
class GeneralInterruptHandle {
    friend auto do_IRQ( IDT::Frame *frame ) -> IDT::Frame *;
    friend IDT;

protected:
    virtual auto name( std::uint64_t vector ) noexcept -> void;
    virtual auto registers( IDT::Frame *frame ) noexcept -> void;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
    virtual auto task( ) noexcept -> void;
    virtual auto cpu( ) noexcept -> void;
    virtual auto handler( IDT::Frame *frame ) noexcept -> IDT::Frame *;

public:
    virtual ~GeneralInterruptHandle( ) = default;

public:
    auto handle( IDT::Frame *frame ) noexcept {
        this->name( frame->vector );
        this->registers( frame );
        this->task( );
        this->cpu( );
        this->error_code( frame->error_code );
        return this->handler( frame );
    }
    static auto register_handle( std::uint64_t vector, GeneralInterruptHandle *handle ) -> void {
        if ( handle ) {
            handlers[ vector ] = handle;
        }
    };

private:
    constexpr static std::string_view error_name[] {
        "#DE Divide-by-zero",
        "#DB Debug",
        "--- Non Maskable Interrupt(NMI)",
        "#BP Breakpoint",
        "#OF Overflow",
        "#BR Bound Range Exceeded",
        "#UD Invalid Opcode",
        "#NM DeviceNotAvailable",
        "#DF Double Fault",
        "--- Coprocessor Segment Overrun",
        "#TS Invalid TSS",
        "#NP Segment Not Present",
        "#SS Stack Segment Fault",
        "#GP General Protection Fault",
        "#PF Page Fault",
        "--- Intel reserved. Do not use.",
        "#MF x87 Floating-Point Exception",
        "#AC Alignment Check",
        "#MC Machine Check",
        "#XM/#XF SIMD Floating-Point Exception",
        "#VE Virtualization Exception",
        "#CP Control Protection Exception",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "#HV Hypervisor Injection Exception",
        "#HV VMM Communication Exception",
        "#SX Security Exception",
        "Reserved",
    };
    inline static GeneralInterruptHandle *handlers[ 256 ] { };
};
class PageFault : public GeneralInterruptHandle {
public:
    virtual ~PageFault( ) = default;

    struct ErrorCode {
        uint32_t present : 1;
        uint32_t write : 1;
        uint32_t user : 1;
        uint32_t reserved_write : 1;
        uint32_t instruction_fetch : 1;
        uint32_t protection_key : 1;
        uint32_t shadow_stack : 1;
        uint32_t : 8;
        uint32_t software_guard_extensions : 1;
        uint32_t : 15;
        uint32_t RMP : 1;
        ErrorCode( uint64_t error_code ) {
            *reinterpret_cast< uint64_t * >( this ) = error_code;
        }
    };

public:
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
};

class InvalidTSS : public GeneralInterruptHandle {
public:
    explicit InvalidTSS( void ) noexcept = default;
    virtual ~InvalidTSS( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
    //  The Invalid TSS exception sets an error code, which is a selector index.
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
class SegmentNotPresent : public GeneralInterruptHandle {
public:
    explicit SegmentNotPresent( void ) noexcept = default;
    virtual ~SegmentNotPresent( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
    // The Segment Not Present exception sets an error code, which is the segment selector index of the segment descriptor which caused the exception.
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
class StackSegmentFault : public GeneralInterruptHandle {
public:
    explicit StackSegmentFault( void ) noexcept = default;
    virtual ~StackSegmentFault( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
    //  The Stack-Segment Fault sets an error code, which is the stack segment selector index when a non-present segment descriptor was referenced or a limit check failed during a hardware task switch. Otherwise (for present segments and already in use), the error code is 0.
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
class GeneralProtectionFault : public GeneralInterruptHandle {
public:
    explicit GeneralProtectionFault( void ) noexcept = default;
    virtual ~GeneralProtectionFault( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
    // The General Protection Fault sets an error code, which is the segment selector index when the exception is segment related. Otherwise, 0.
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
class ControlProtectionException : public GeneralInterruptHandle {
public:
    explicit ControlProtectionException( void ) noexcept = default;
    virtual ~ControlProtectionException( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
    using ErrorCode = uint64_t;
};
class HypervisorInjectionException : public GeneralInterruptHandle {
public:
    explicit HypervisorInjectionException( void ) noexcept = default;
    virtual ~HypervisorInjectionException( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
};
class VMMCommunicationException : public GeneralInterruptHandle {
public:
    explicit VMMCommunicationException( void ) noexcept = default;
    virtual ~VMMCommunicationException( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
};
class SecurityException : public GeneralInterruptHandle {
public:
    explicit SecurityException( void ) noexcept = default;
    virtual ~SecurityException( void ) noexcept = default;
    virtual auto error_code( std::uint64_t code ) noexcept -> void;
};

}     // namespace Interrupt