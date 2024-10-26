#pragma once
#include <lib/Uefi.hpp>
#include <type_traits>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    /**
     * @brief 描述符管理模板
     * @tparam DescriptorType 要管理的描述符
     */
    PUBLIC template < typename DescriptorType, uint64_t table_count, uint64_t descriptor_count >
        requires std::is_compound_v< DescriptorType >
    class Descriptor
    {
    public:
        explicit Descriptor( VOID ) noexcept {
            for ( auto i { 0ull }; i < table_count; ++i ) {
                this->xdtr[ i ].size = uint16_t( sizeof( DescriptorType ) * descriptor_count - 1 );
                this->xdtr[ i ].descriptor = this->descriptor_table[ i ];
            }
        }
        virtual ~Descriptor( VOID ) noexcept = default;

    public:
        /**
         * @brief 装载描述符
         */
        virtual auto load( IN uint64_t processor_id ) const -> VOID = 0;
        /**
         * @brief 读取
         * @return 指向Descriptor Table的指针
         */
        virtual auto read( IN uint64_t processor_id ) const -> DescriptorType * = 0;

        /**
         * @brief 描述符表引索
         */
        struct DescriptorRegister
        {
            uint16_t size { };
            DescriptorType *descriptor { };
        } _packed xdtr[ table_count ];

        DescriptorType descriptor_table[ table_count ][ descriptor_count ];
    };
}
