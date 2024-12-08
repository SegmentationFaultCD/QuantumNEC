#pragma once
#include <kernel/interrupt/x86_64/entry/entry.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class DivisionError : public InterruptEntry {
public:
    explicit DivisionError( void ) noexcept = default;
    virtual ~DivisionError( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class Debug : public InterruptEntry {
public:
    explicit Debug( void ) noexcept = default;
    virtual ~Debug( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class NonMaskableInterrupt : public InterruptEntry {
public:
    explicit NonMaskableInterrupt( void ) noexcept = default;
    virtual ~NonMaskableInterrupt( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class Breakpoint : public InterruptEntry {
public:
    explicit Breakpoint( void ) noexcept = default;
    virtual ~Breakpoint( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class Overflow : public InterruptEntry {
public:
    explicit Overflow( void ) noexcept = default;
    virtual ~Overflow( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class BoundRangeExceeded : public InterruptEntry {
public:
    explicit BoundRangeExceeded( void ) noexcept = default;
    virtual ~BoundRangeExceeded( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class InvalidOpcode : public InterruptEntry {
public:
    explicit InvalidOpcode( void ) noexcept = default;
    virtual ~InvalidOpcode( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class DeviceNotAvailable : public InterruptEntry {
public:
    explicit DeviceNotAvailable( void ) noexcept = default;
    virtual ~DeviceNotAvailable( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class DoubleFault : public InterruptEntry {
public:
    explicit DoubleFault( void ) noexcept = default;
    virtual ~DoubleFault( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class CoprocessorSegmentOverrun : public InterruptEntry {
public:
    explicit CoprocessorSegmentOverrun( void ) noexcept = default;
    virtual ~CoprocessorSegmentOverrun( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class InvalidTSS : public InterruptEntry {
public:
    explicit InvalidTSS( void ) noexcept = default;
    virtual ~InvalidTSS( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }

    //  The Invalid TSS exception sets an error code, which is a selector index.
};
class SegmentNotPresent : public InterruptEntry {
public:
    explicit SegmentNotPresent( void ) noexcept = default;
    virtual ~SegmentNotPresent( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }

    // The Segment Not Present exception sets an error code, which is the segment selector index of the segment descriptor which caused the exception.
};
class StackSegmentFault : public InterruptEntry {
public:
    explicit StackSegmentFault( void ) noexcept = default;
    virtual ~StackSegmentFault( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }

private:
    //  The Stack-Segment Fault sets an error code, which is the stack segment selector index when a non-present segment descriptor was referenced or a limit check failed during a hardware task switch. Otherwise (for present segments and already in use), the error code is 0.
};
class GeneralProtectionFault : public InterruptEntry {
public:
    explicit GeneralProtectionFault( void ) noexcept = default;
    virtual ~GeneralProtectionFault( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }

    // The General Protection Fault sets an error code, which is the segment selector index when the exception is segment related. Otherwise, 0.
};
class PageFault : public InterruptEntry {
public:
    explicit PageFault( void ) noexcept = default;
    virtual ~PageFault( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }

private:
    struct PFErrorCode {
        uint32_t present : 1;
        uint32_t write : 1;
        uint32_t user : 1;
        uint32_t reserved_write : 1;
        uint32_t instruction_fetch : 1;
        uint32_t protection_key : 1;
        uint32_t shadow_stack : 1;
        uint32_t : 8;
        uint32_t software_guard_extensions : 1;
        uint32_t : 16;
        PFErrorCode( uint64_t error_code ) {
            *reinterpret_cast< uint64_t * >( this ) = error_code;
        }
    };
};
// 0x15 Intel Reserved

class X87FloatingPointException : public InterruptEntry {
public:
    explicit X87FloatingPointException( void ) noexcept = default;
    virtual ~X87FloatingPointException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class AlignmentCheck : public InterruptEntry {
public:
    explicit AlignmentCheck( void ) noexcept = default;
    virtual ~AlignmentCheck( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class MachinCheck : public InterruptEntry {
public:
    explicit MachinCheck( void ) noexcept = default;
    virtual ~MachinCheck( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class SIMDFloatingPointException : public InterruptEntry {
public:
    explicit SIMDFloatingPointException( void ) noexcept = default;
    virtual ~SIMDFloatingPointException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class VirtualizationException : public InterruptEntry {
public:
    explicit VirtualizationException( void ) noexcept = default;
    virtual ~VirtualizationException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class ControlProtectionException : public InterruptEntry {
public:
    explicit ControlProtectionException( void ) noexcept = default;
    virtual ~ControlProtectionException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }

private:
    struct CPErrorCode {
    };
};

// 0x16-0x1B Reserved

class HypervisorInjectionException : public InterruptEntry {
public:
    explicit HypervisorInjectionException( void ) noexcept = default;
    virtual ~HypervisorInjectionException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto error_code( uint64_t error_code ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class VMMCommunicationException : public InterruptEntry {
public:
    explicit VMMCommunicationException( void ) noexcept = default;
    virtual ~VMMCommunicationException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
class SecurityException : public InterruptEntry {
public:
    explicit SecurityException( void ) noexcept = default;
    virtual ~SecurityException( void ) noexcept = default;

private:
    virtual auto name( void ) noexcept -> void override;
    virtual auto handler( Frame *frame ) noexcept -> Frame * override;
    virtual auto do_register( void ) -> void override {
    }
};
// 0x1F Reserved
}     // namespace QuantumNEC::Kernel::x86_64