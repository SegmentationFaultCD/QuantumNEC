#pragma once
#include <lib/Uefi.hpp>
#if defined( __x86_64__ )
#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/interrupt/arch/x86_64/pit/pit.hpp>
#elif defined( __aarch64__ )
#include <kernel/interrupt/arch/aarch64/entry/entry.hpp>
#include <kernel/interrupt/arch/aarch64/entry/idt.hpp>
#include <kernel/interrupt/arch/aarch64/pic/pic.hpp>
#include <kernel/interrupt/arch/aarch64/pit/pit.hpp>
#endif

PUBLIC namespace QuantumNEC::Kernel {
#if defined( __x86_64__ )
    using namespace x86_64;
#elif defined( __aarch64__ )
    using namespace aarch64;
#endif
    class InterruptArch :
#if defined( __x86_64__ )
        public InterruptDescriptorTable,
        public PIC,
        public PIT,
        public InterruptEntryRegister
#elif defined( __aarch64__ )
#endif
    {
    public:
        enum class InterruptStatus : BOOL {
            INTERRUPT_DISABLE = FALSE, /* 关闭 */
            INTERRUPT_ENABLE = TRUE    /* 打开 */
        };

    public:
        explicit InterruptArch( VOID ) noexcept = default;

    public:
        /**
         * @brief 开中断
         */
        STATIC auto enable_interrupt( VOID ) -> InterruptStatus;
        /**
         * @brief 关中断
         */
        STATIC auto disable_interrupt( VOID ) -> InterruptStatus;
        /**
         * @brief 设置中断状态
         */
        STATIC auto set_interrupt( IN CONST InterruptStatus &status ) -> InterruptStatus;
        /**
         * @brief 获取中断状态
         */
        STATIC auto get_interrupt( VOID ) -> InterruptStatus;
    };
}