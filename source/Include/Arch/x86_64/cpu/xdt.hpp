#pragma once
#include <Arch/x86_64/interrupt/interrupt.hpp>
#include <Arch/x86_64/platform/descriptor.hpp>
#include <Arch/x86_64/platform/global.hpp>
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    /**
     * @brief 中断描述符
     */
    PUBLIC struct InterruptDescriptor
    {
    private:
        Lib::Types::uint16_t offset_low;    /* 偏移15~0 */
        Lib::Types::uint16_t selector;      /* 目标代码段选择子 */
        Lib::Types::uint8_t ist;            /* ist和空值 */
        Lib::Types::uint8_t attribute;      /* 属性 */
        Lib::Types::uint16_t offset_middle; /* 偏移31~16 */
        Lib::Types::uint32_t offset_high;   /* 偏移63~32 */
        Lib::Types::uint32_t reserved;      /* 保留 */
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
            IN CONST Lib::Types::uint64_t entry_point,
            IN CONST Lib::Types::uint16_t selector,
            IN CONST Lib::Types::uint8_t ist,
            IN CONST Lib::Types::uint8_t attributes ) -> Lib::Types::L_Ref< InterruptDescriptor >;
    } _packed;
    /**
     * @brief 中断描述符表管理器
     */
    PUBLIC class InterruptDescriptorTable :
        Platform::Descriptor< InterruptDescriptor, INTERRUPT_DESCRIPTOR_TABLE_COUNT, INTERRUPT_DESCRIPTOR_COUNT >
    {
    private:
        /**
         * @brief 中断描述符属性
         */
        enum class InterruptDescriptorAttribute {
            INTERRUPT = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL0 << 5 ) + IDT_DESC_32_TYPE,            // P,DPL=0,TYPE=E
            TRAP = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL0 << 5 ) + IDT_DESC_64_TYPE,                 // P,DPL=0,TYPE=F
            SYSTEM_INTERRUPT = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL3 << 5 ) + IDT_DESC_32_TYPE,     // P,DPL=3,TYPE=E
            SYSTEM = ( IDT_DESC_P << 7 ) + ( IDT_DESC_DPL3 << 5 ) + IDT_DESC_64_TYPE,               // P,DPL=3,TYPE=F
        };

    public:
        /* 寄存器信息. */
        PUBLIC struct InterruptFrame
        {
            RegisterFrame regs;
            Lib::Types::uint64_t vector;
            Lib::Types::uint64_t error_code;
            Lib::Types::Ptr< VOID > rip;
            Lib::Types::uint64_t cs;
            Lib::Types::uint64_t rflags;
            Lib::Types::uint64_t rsp;
            Lib::Types::uint64_t ss;
        } _packed;

    public:
        explicit InterruptDescriptorTable( VOID ) noexcept;
        virtual ~InterruptDescriptorTable( VOID ) noexcept = default;

    public:
        /**
         * @brief 载入IDT
         */
        virtual auto load( VOID ) const -> VOID override;
        /**
         * @brief 读取IDT
         * @return 中断描述表开头地址
         */
        virtual auto read( VOID ) const -> Lib::Types::Ptr< InterruptDescriptor > override;

    public:
        using InterruptEntry = Lib::Types::FuncPtr< Lib::Types::Ptr< CONST InterruptFrame >, Lib::Types::Ptr< CONST InterruptFrame > >;
        /**
         * @brief 输出寄存器状态
         * @param regs 寄存器数据
         */
        STATIC auto display_registers( IN Lib::Types::Ptr< CONST RegisterFrame > registers ) -> VOID;
        /**
         * @brief 设置中断入口函数
         * @param nr 中断号
         * @param handle 要注册的入口函数指针
         */
        STATIC auto set_interrupt_handler( IN CONST Lib::Types::uint8_t irq, IN InterruptEntry handle ) -> VOID;
        /**
         * @brief 设置异常入口函数
         * @param nr 中断号
         * @param handle 要注册的入口函数指针
         */
        STATIC auto set_exception_handler( IN CONST Lib::Types::uint8_t vector, IN InterruptEntry handle ) -> VOID;
    };
    /**
     * @brief 全局段描述符
     */
    PUBLIC struct GlobalSegmentDescriptor
    {
    private:
        /*  全局段描述符内部安排
            8192个一组
            1 ~ 10 段描述符(SD)
            11 任务状态段描述符(TSS)低地址
            12 任务状态段描述符(TSS)高地址
            13 ~ 8192 保留，作为以后扩充
        */
        Lib::Types::uint16_t limit_low;       // 0-15 limit1
        Lib::Types::uint16_t base_low;        // 16 - 31 base0
        Lib::Types::uint8_t base_middle;      // 32 - 39 base1
        Lib::Types::uint8_t access_right;     // 40 - 47 flag descType privilege isVaild
        Lib::Types::uint8_t limit_high;       // 48 - 55 limit1 usused
        Lib::Types::uint8_t base_high;        // 56 - 63 base2
    public:
        /**
         * @brief 为全局描述符制作段描述符
         * @retval 制作好的全局描述符
         */
        auto make(
            IN Lib::Types::uint64_t base,
            IN Lib::Types::uint64_t limit,
            IN Lib::Types::uint64_t access ) -> Lib::Types::L_Ref< GlobalSegmentDescriptor >;
    } _packed;
    /**
     * @brief 任务状态段描述符
     */
    PUBLIC struct TaskStateSegmentDescriptor64
    {
    private:
        Lib::Types::uint32_t reserved1;
        Lib::Types::uint64_t rsp[ 3 ];
        Lib::Types::uint64_t reserved2;
        Lib::Types::uint64_t ist[ 7 ];
        Lib::Types::uint64_t reserved3;
        Lib::Types::uint32_t io_map_base_address;

    public:
        auto get( VOID ) noexcept -> Lib::Types::L_Ref< CONST TaskStateSegmentDescriptor64 > {
            return *this;
        }
        template < Lib::Types::uint64_t task_stack_size >
        auto set_rsp0( Lib::Types::Ptr< VOID > pcb ) noexcept {
            this->rsp[ 0 ] = reinterpret_cast< Lib::Types::uint64_t >( pcb ) + task_stack_size;
        }
        auto set_io_map_base_address( IN Lib::Types::uint16_t io_map_base_address ) noexcept {
            this->io_map_base_address = io_map_base_address;
        }
        auto load_tr( IN Lib::Types::size_t segment ) noexcept {
            ASM( "ltr %%ax" ::"a"( segment ) : "memory" );
        }
    } _packed;
    using TaskStateSegmentDescriptor = TaskStateSegmentDescriptor64;
    /**
     * @brief 全局段描述符表
     */
    PUBLIC class GlobalSegmentDescriptorTable :
        Platform::Descriptor< GlobalSegmentDescriptor, GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT, SEGMENT_DESCRIPTOR_COUNT >
    {
    public:
        explicit GlobalSegmentDescriptorTable( VOID ) noexcept;
        virtual ~GlobalSegmentDescriptorTable( VOID ) noexcept = default;

    public:
        /**
         * @brief 载入GDT
         */
        virtual auto load( VOID ) CONST->VOID override;
        /**
         * @brief 读取GDT
         * @return 指向全局描述符表开头的地址
         */
        virtual auto read( VOID ) CONST->Lib::Types::Ptr< GlobalSegmentDescriptor > override;

    public:
        inline STATIC TaskStateSegmentDescriptor64 tss[ TASK_STATE_SEGMENT_DESCRIPTOR_COUNT ] { };
    };
}