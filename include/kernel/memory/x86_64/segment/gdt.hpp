#pragma once
#include <kernel/global/x86_64/descriptor.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
/**
 * @brief 全局段描述符
 */
struct _packed GlobalSegmentDescriptor {
private:
    /*  全局段描述符内部安排
        8192个一组
        1 ~ 10 段描述符(SD)
        11 任务状态段描述符(TSS)低地址
        12 任务状态段描述符(TSS)高地址
        13 ~ 8192 保留，作为以后扩充
    */
    uint16_t limit_low;        // 0-15 limit1
    uint16_t base_low;         // 16 - 31 base0
    uint8_t  base_middle;      // 32 - 39 base1
    uint8_t  access_right;     // 40 - 47 flag descType privilege isVaild
    uint8_t  limit_high;       // 48 - 55 limit1 usused
    uint8_t  base_high;        // 56 - 63 base2 ’

public:
    explicit GlobalSegmentDescriptor( void ) noexcept = default;

public:
    /**
     * @brief 为全局描述符制作段描述符
     * @retval 制作好的全局描述符
     */
    auto make(
        uint64_t base,
        uint64_t limit,
        uint64_t access ) -> GlobalSegmentDescriptor &;
};
/**
 * @brief 任务状态段描述符
 */
struct _packed TaskStateSegmentDescriptor64 {
private:
    uint32_t reserved1;
    uint64_t rsp[ 3 ];
    uint64_t reserved2;
    uint64_t ist[ 7 ];
    uint64_t reserved3;
    uint32_t io_map_base_address;

public:
    explicit TaskStateSegmentDescriptor64( void ) noexcept {
    }

public:
    auto get_rsp0( void ) noexcept -> uint64_t {
        return this->rsp[ 0 ];
    }
    auto set_rsp0( uint64_t stack_top ) noexcept {
        this->rsp[ 0 ] = stack_top;
    }
    auto set_io_map_base_address( IN uint16_t io_map_base_address ) noexcept {
        this->io_map_base_address = io_map_base_address;
    }
    auto load_tr( IN size_t segment ) noexcept {
        ASM( "ltr %%ax" ::"a"( segment )
             : "memory" );
    }
};
using TaskStateSegmentDescriptor = TaskStateSegmentDescriptor64;
/**
 * @brief 全局段描述符表
 */
class GlobalSegmentDescriptorTable {
public:
    explicit GlobalSegmentDescriptorTable( void ) noexcept;
    ~GlobalSegmentDescriptorTable( void ) noexcept = default;

public:
    class _GDT : public Descriptor< GlobalSegmentDescriptor, GLOBAL_SEGMENT_DESCRIPTOR_TABLE_COUNT, SEGMENT_DESCRIPTOR_COUNT > {
    public:
        /**
         * @brief 载入GDT
         */
        virtual auto load( IN uint64_t processor_id ) const -> void override;
        /**
         * @brief 读取GDT
         * @return 指向全局描述符表开头的地址
         */
        virtual auto read( IN uint64_t processor_id ) const -> GlobalSegmentDescriptor * override;

        TaskStateSegmentDescriptor64 tss[ TASK_STATE_SEGMENT_DESCRIPTOR_COUNT ];
    };
    inline static _GDT *gdt;
};
}     // namespace QuantumNEC::Kernel::x86_64