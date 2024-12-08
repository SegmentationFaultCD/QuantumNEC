#pragma once
#ifndef APIC
#define APIC
#endif
#include <lib/Uefi.hpp>

#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/entry/register.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/interrupt/x86_64/pit/pit.hpp>
#elif defined( __aarch64__ )
#endif

namespace QuantumNEC::Kernel {
class Interrupt :
#if defined( __x86_64__ )
    public x86_64::InterruptDescriptorTable,
    public x86_64::PIC,
    public x86_64::PIT,
    public x86_64::InterruptEntryRegister
#elif defined( __aarch64__ )
#endif
{
public:
    enum class InterruptStatus : bool {
        DISABLE = false, /* 关闭 */
        ENABLE  = true   /* 打开 */
    };

public:
    explicit Interrupt( void ) noexcept = default;

public:
    /**
     * @brief 开中断
     */
    static auto enable_interrupt( void ) -> InterruptStatus;
    /**
     * @brief 关中断
     */
    static auto disable_interrupt( void ) -> InterruptStatus;
    /**
     * @brief 设置中断状态
     */
    static auto set_interrupt( IN const InterruptStatus &status ) -> InterruptStatus;
    /**
     * @brief 获取中断状态
     */
    static auto get_interrupt( void ) -> InterruptStatus;
};
}     // namespace QuantumNEC::Kernel
