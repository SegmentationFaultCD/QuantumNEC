#pragma once
#include <Lib/Base/deflib.hpp>
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <type_traits>
PUBLIC namespace QuantumNEC::Architecture::Platform {
    /**
     * @brief 描述符管理模板
     * @tparam D 要管理的描述符，约定必须有一个set成员函数，接受一个D*参数，返回D&
     */
    PUBLIC template < typename DescriptorType, Lib::Types::uint64_t table_count, Lib::Types::uint64_t descriptor_count >
        requires std::is_compound_v< DescriptorType >
    class Descriptor
    {
    public:
        explicit Descriptor( VOID ) noexcept :
            xdtr { Lib::Types::uint16_t( sizeof( DescriptorType ) * descriptor_count - 1 ), *this->descriptor_table } {
        }
        virtual ~Descriptor( VOID ) noexcept = default;

    public:
        /**
         * @brief 装载描述符
         */
        virtual auto load( VOID ) const -> VOID = 0;
        /**
         * @brief 读取
         * @return 指向Descriptor Table的指针
         */
        virtual auto read( VOID ) const -> Lib::Types::Ptr< DescriptorType > = 0;

    protected:
        /**
         * @brief 描述符表引索
         */
        struct DescriptorRegister
        {
            Lib::Types::uint16_t size { };
            Lib::Types::Ptr< DescriptorType > descriptor { };
        } _packed xdtr;

        inline STATIC DescriptorType descriptor_table[ table_count ][ descriptor_count ];
    };
}
