#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#define BITMAP_MASK 1
PUBLIC namespace QuantumNEC::Lib::Base {
    PUBLIC class Bitmap
    {
    private:
        struct BitMapData
        {
            Lib::Types::uint64_t length;                    // 位图以字节为单位的长度
            Lib::Types::Ptr< Lib::Types::byte_t > bits;     // 位图
        };

    public:
        explicit Bitmap( VOID ) noexcept = default;
        /**
         * @brief 初始化位图
         * @param bit_length 要初始化的位图长度
         * @param bits       要初始化的位图指针
         */
        explicit Bitmap( IN Lib::Types::uint64_t bit_length, IN Lib::Types::Ptr< Lib::Types::byte_t > bits ) noexcept;
        explicit Bitmap( IN Lib::Types::L_Ref< CONST Bitmap > _bitmap ) noexcept;
        virtual ~Bitmap( VOID ) noexcept;

    public:
        auto operator=( IN Lib::Types::L_Ref< CONST Bitmap > _bitmap ) noexcept -> Lib::Types::L_Ref< CONST Bitmap >;

    public:
        /**
         * @brief 判断位图中bit_idx是否为1
         * @param btmp 位图指针
         * @param bit_index bit位下标
         * @return 为1返回true 为0返回false
         */
        auto scan( IN Lib::Types::size_t bit_idx ) -> Lib::Types::BOOL;
        /**
         * @brief 在位图中分配位
         * @param btmp 位图指针
         * @param cnt 要分配的位数
         * @retval 分配成功返回位的下标 分配失败返回-1
         */
        auto allocate( IN Lib::Types::size_t cnt ) -> Lib::Types::int64_t;
        /**
         * @brief 将位图的bit_index位设为value
         * @param btmp 位图指针
         * @param bit_index 要设置的位的下标
         * @param value 设置值
         */
        auto set( IN Lib::Types::size_t bit_idx, IN Lib::Types::int8_t value ) -> VOID;

    public:
        auto set_length( IN Lib::Types::uint64_t bit_length ) -> VOID {
            this->bitmap_.length = bit_length;
        }
        auto set_bits( IN Lib::Types::Ptr< Lib::Types::byte_t > bits ) -> VOID {
            this->bitmap_.bits = bits;
        }

    private:
        BitMapData bitmap_ { };
    };
}