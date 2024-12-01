#pragma once
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class DivisionError : public InterruptEntry {
    public:
        explicit DivisionError( VOID ) noexcept = default;
        virtual ~DivisionError( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class Debug : public InterruptEntry {
    public:
        explicit Debug( VOID ) noexcept = default;
        virtual ~Debug( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class NonMaskableInterrupt : public InterruptEntry {
    public:
        explicit NonMaskableInterrupt( VOID ) noexcept = default;
        virtual ~NonMaskableInterrupt( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class Breakpoint : public InterruptEntry {
    public:
        explicit Breakpoint( VOID ) noexcept = default;
        virtual ~Breakpoint( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class Overflow : public InterruptEntry {
    public:
        explicit Overflow( VOID ) noexcept = default;
        virtual ~Overflow( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class BoundRangeExceeded : public InterruptEntry {
    public:
        explicit BoundRangeExceeded( VOID ) noexcept = default;
        virtual ~BoundRangeExceeded( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class InvalidOpcode : public InterruptEntry {
    public:
        explicit InvalidOpcode( VOID ) noexcept = default;
        virtual ~InvalidOpcode( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class DeviceNotAvailable : public InterruptEntry {
    public:
        explicit DeviceNotAvailable( VOID ) noexcept = default;
        virtual ~DeviceNotAvailable( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class DoubleFault : public InterruptEntry {
    public:
        explicit DoubleFault( VOID ) noexcept = default;
        virtual ~DoubleFault( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class CoprocessorSegmentOverrun : public InterruptEntry {
    public:
        explicit CoprocessorSegmentOverrun( VOID ) noexcept = default;
        virtual ~CoprocessorSegmentOverrun( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class InvalidTSS : public InterruptEntry {
    public:
        explicit InvalidTSS( VOID ) noexcept = default;
        virtual ~InvalidTSS( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }

        //  The Invalid TSS exception sets an error code, which is a selector index.
    };
    PUBLIC class SegmentNotPresent : public InterruptEntry {
    public:
        explicit SegmentNotPresent( VOID ) noexcept = default;
        virtual ~SegmentNotPresent( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }

        // The Segment Not Present exception sets an error code, which is the segment selector index of the segment descriptor which caused the exception.
    };
    PUBLIC class StackSegmentFault : public InterruptEntry {
    public:
        explicit StackSegmentFault( VOID ) noexcept = default;
        virtual ~StackSegmentFault( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }

    private:
        //  The Stack-Segment Fault sets an error code, which is the stack segment selector index when a non-present segment descriptor was referenced or a limit check failed during a hardware task switch. Otherwise (for present segments and already in use), the error code is 0.
    };
    PUBLIC class GeneralProtectionFault : public InterruptEntry {
    public:
        explicit GeneralProtectionFault( VOID ) noexcept = default;
        virtual ~GeneralProtectionFault( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }

        // The General Protection Fault sets an error code, which is the segment selector index when the exception is segment related. Otherwise, 0.
    };
    PUBLIC class PageFault : public InterruptEntry {
    public:
        explicit PageFault( VOID ) noexcept = default;
        virtual ~PageFault( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
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
                *this = *reinterpret_cast< PFErrorCode * >( error_code );
            }
        };
    };
    // 0x15 Intel Reserved

    PUBLIC class X87FloatingPointException : public InterruptEntry {
    public:
        explicit X87FloatingPointException( VOID ) noexcept = default;
        virtual ~X87FloatingPointException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class AlignmentCheck : public InterruptEntry {
    public:
        explicit AlignmentCheck( VOID ) noexcept = default;
        virtual ~AlignmentCheck( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class MachinCheck : public InterruptEntry {
    public:
        explicit MachinCheck( VOID ) noexcept = default;
        virtual ~MachinCheck( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class SIMDFloatingPointException : public InterruptEntry {
    public:
        explicit SIMDFloatingPointException( VOID ) noexcept = default;
        virtual ~SIMDFloatingPointException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class VirtualizationException : public InterruptEntry {
    public:
        explicit VirtualizationException( VOID ) noexcept = default;
        virtual ~VirtualizationException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class ControlProtectionException : public InterruptEntry {
    public:
        explicit ControlProtectionException( VOID ) noexcept = default;
        virtual ~ControlProtectionException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };

    // 0x16-0x1B Reserved

    PUBLIC class HypervisorInjectionException : public InterruptEntry {
    public:
        explicit HypervisorInjectionException( VOID ) noexcept = default;
        virtual ~HypervisorInjectionException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }
    };
    PUBLIC class VMMCommunicationException : public InterruptEntry {
    public:
        explicit VMMCommunicationException( VOID ) noexcept = default;
        virtual ~VMMCommunicationException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }

    private:
        struct ErrorCode {
            uint32_t external : 1;
            uint32_t tbl : 2;
            uint32_t index : 13;
            uint32_t : 16;
            ErrorCode( uint64_t error_code ) {
                *this = *reinterpret_cast< ErrorCode * >( error_code );
            }
        };
    };
    PUBLIC class SecurityException : public InterruptEntry {
    public:
        explicit SecurityException( VOID ) noexcept = default;
        virtual ~SecurityException( VOID ) noexcept = default;

    private:
        virtual auto name( VOID ) noexcept -> VOID override;
        virtual auto handler( Frame *frame ) noexcept -> Frame * override;
        virtual auto do_register( VOID ) -> VOID override {
        }

    private:
        struct ErrorCode {
            uint32_t external : 1;
            uint32_t tbl : 2;
            uint32_t index : 13;
            uint32_t : 16;
            ErrorCode( uint64_t error_code ) {
                *this = *reinterpret_cast< ErrorCode * >( error_code );
            }
        };
    };
    // 0x1F Reserved
}