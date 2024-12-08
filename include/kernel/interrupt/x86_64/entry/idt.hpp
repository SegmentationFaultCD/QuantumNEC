#pragma once
#include <kernel/global/x86_64/descriptor.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <lib/Uefi.hpp>

namespace QuantumNEC::Kernel::x86_64 {
/**
 * @brief 中断描述符
 */
struct _packed InterruptDescriptor {
private:
    uint16_t offset_low;    /* 偏移15~0 */
    uint16_t selector;      /* 目标代码段选择子 */
    uint8_t  ist;           /* ist和空值 */
    uint8_t  attribute;     /* 属性 */
    uint16_t offset_middle; /* 偏移31~16 */
    uint32_t offset_high;   /* 偏移63~32 */
    uint32_t reserved;      /* 保留 */
public:
    explicit InterruptDescriptor( void ) noexcept = default;

public:
    /**
     * @brief 设置中断描述符
     * @param entry_point 中断入口函数头
     * @param selector 选择器
     * @param ist 状态
     * @param attributes 属性
     * @retval 构建好的中断描述符
     */
    auto make(
        IN uint64_t entry_point,
        IN uint16_t selector,
        IN uint8_t  ist,
        IN uint8_t  attributes ) -> InterruptDescriptor &;
};
/**
 * @brief 中断描述符表管理器
 */
class InterruptDescriptorTable {
private:
    /**
     * @brief 中断描述符属性
     */
    enum class InterruptDescriptorAttribute {
        INTERRUPT        = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL0 << 5 ) + IDT_DESC_32_TYPE,     // P,DPL=0,TYPE=E
        TRAP             = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL0 << 5 ) + IDT_DESC_64_TYPE,     // P,DPL=0,TYPE=F
        SYSTEM_INTERRUPT = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL3 << 5 ) + IDT_DESC_32_TYPE,     // P,DPL=3,TYPE=E
        SYSTEM           = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL3 << 5 ) + IDT_DESC_64_TYPE,     // P,DPL=3,TYPE=F
    };

public:
    struct InterruptFrame;
    using InterruptEntry = FuncPtr< const InterruptFrame *, const InterruptFrame *, uint64_t >;
    /* 寄存器信息. */
    struct _packed InterruptFrame {
        RegisterFrame  regs;
        InterruptEntry handler;
        uint64_t       old_rax;
        uint64_t       vector;
        uint64_t       error_code;
        void          *rip;
        uint64_t       cs;
        RFlags         rflags;
        uint64_t       rsp;
        uint64_t       ss;
        explicit InterruptFrame( void ) noexcept = default;
    };
    struct InterruptFunctionController {
        FuncPtr< uint64_t, uint64_t, void * > install;
        FuncPtr< void, uint64_t >             uninstall;
        FuncPtr< void, uint64_t >             enable;
        FuncPtr< void, uint64_t >             disable;
        FuncPtr< void, uint64_t >             ack;
    };
    struct InterruptFunctionTable {
        InterruptFunctionController controller;
        const char_t               *irq_name;
        uint64_t                    parameter;
        InterruptEntry              handler;
        uint64_t                    flags;
        explicit InterruptFunctionTable( void ) noexcept = default;
    };

public:
    explicit InterruptDescriptorTable( void ) noexcept;
    virtual ~InterruptDescriptorTable( void ) noexcept = default;

public:
    class _IDT : public Descriptor< InterruptDescriptor, INTERRUPT_DESCRIPTOR_TABLE_COUNT, INTERRUPT_DESCRIPTOR_COUNT > {
    public:
        explicit _IDT( void ) = default;

    public:
        /**
         * @brief 载入IDT
         */
        virtual auto load( IN uint64_t processor_id ) const -> void override;
        /**
         * @brief 读取IDT
         * @return 中断描述表开头地址
         */
        virtual auto read( IN uint64_t processor_id ) const -> InterruptDescriptor * override;
    };
    inline static _IDT *idt;
};
}     // namespace QuantumNEC::Kernel::x86_64