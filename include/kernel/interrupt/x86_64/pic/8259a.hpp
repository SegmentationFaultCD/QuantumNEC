#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class PIC8259A {
private:
    using irq_t = uint64_t;

public:
    explicit PIC8259A( void ) noexcept;

public:
    virtual ~PIC8259A( void ) noexcept;

public:
    static auto eoi( IN const irq_t irq ) -> void;
    static auto disable_8259A_pic( void ) -> void;
    static auto enable_8259A_pic( void ) -> void;
    static auto irq_set_mask( IN irq_t irq ) -> void;
    static auto irq_clear_mask( IN irq_t irq ) -> void;
    static auto get_irr( void ) -> uint16_t;
    static auto get_isr( void ) -> uint16_t;

private:
    static auto get_irq_reg( IN int32_t ocw3 ) -> uint16_t;
};
}     // namespace QuantumNEC::Kernel::x86_64