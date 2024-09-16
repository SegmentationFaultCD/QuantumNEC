#include <Lib/Base/bitmap.hpp>
#include <Lib/IO/Stream/iostream>
#include <Lib/STL/string>
PUBLIC namespace QuantumNEC::Lib::Base {
    Bitmap::Bitmap( IN Lib::Types::uint64_t bit_length, IN Lib::Types::Ptr< Lib::Types::byte_t > bits ) noexcept {
        bitmap_.length = bit_length;
        bitmap_.bits = bits;
    }
    Bitmap::Bitmap( IN Lib::Types::L_Ref< CONST Bitmap > _bitmap ) noexcept {
        this->bitmap_.bits = _bitmap.bitmap_.bits;
        this->bitmap_.length = _bitmap.bitmap_.length;
    }
    auto Bitmap::operator=( IN Lib::Types::L_Ref< CONST Bitmap > _bitmap ) noexcept -> Lib::Types::L_Ref< CONST Bitmap > {
        this->bitmap_.bits = _bitmap.bitmap_.bits;
        this->bitmap_.length = _bitmap.bitmap_.length;
        return *this;
    }
    Bitmap::~Bitmap( VOID ) noexcept {
    }
    auto Bitmap::scan( Lib::Types::size_t bit_idx )->Lib::Types::BOOL {
        return this->bitmap_.bits[ bit_idx / 8 ] & ( BITMAP_MASK << ( bit_idx % 8 ) );
    }
    auto Bitmap::allocate( Lib::Types::size_t cnt )->Types::int64_t {
        Lib::Types::size_t index { };
        /* 寻找第一个空的bit所在位 */
        /* 位图已满,找不到空位 */
        while ( index < this->bitmap_.length && this->bitmap_.bits[ index ] == 0xff ) {
            index++;
        }
        if ( index >= this->bitmap_.length || index == this->bitmap_.length ) {
            return -1; /* 分配失败 */
        }
        /* 发现有空位,*/
        Lib::Types::size_t bit_index { };
        /* 位图中寻找空闲位的索引 */
        while ( ( ( Lib::Types::uint8_t )( BITMAP_MASK << bit_index ) ) & this->bitmap_.bits[ index ] ) {
            bit_index++;
        }
        Lib::Types::int64_t bit_index_start { static_cast< Lib::Types::int64_t >( index * 8 + bit_index ) }; /* 空闲位在整个位图中的下标 */
        /* 只分配一个位, 那就是bit_index_start位 */
        if ( cnt == 1 ) {
            return bit_index_start;
        }
        Lib::Types::size_t bit_free { this->bitmap_.length * 8 - bit_index_start }; /* 剩下的位 */
        Lib::Types::int64_t next_bit_index { bit_index_start + 1 };
        Lib::Types::size_t count { 1 }; /* 找到的空闲位的个数,上面已经找到一个了 */
        bit_index_start = -1;           /* 找不到时直接返回-1 */
        while ( bit_free > 0 ) {
            if ( !( this->scan( next_bit_index ) ) ) {
                /* 下一个bit位是0,那就又找到一个空bit位 */
                count++;
            }
            else {
                /* next_bit_index位不是0,重新找空闲位 */
                count = 0;
            }
            if ( count == cnt ) {
                /* 找到了连续cnt个空闲位 */
                bit_index_start = next_bit_index - cnt + 1; /* 空闲位的起始下标 */
                break;                                      /* 寻找结束,退出循环 */
            }
            bit_free--;
            next_bit_index++;
        }
        return bit_index_start;
    }
    auto Bitmap::set( Lib::Types::size_t bit_idx, Lib::Types::int8_t value )->VOID {
        /* 一般都会用个 0x1 这样的数对字节中的位操作 74 * 将 1
         * 任意移动后再取反，或者先取反再移位，可用来对位置 0 操作。*/
        switch ( value ) {
        case 0:     // 如果 value 为 0
            this->bitmap_.bits[ bit_idx / 8 ] &= ( ~( BITMAP_MASK << bit_idx % 8 ) );
            break;
        case 1:     // 如果 value 为 1
            this->bitmap_.bits[ bit_idx / 8 ] |= ( BITMAP_MASK << bit_idx % 8 );
            break;
        default:
            break;
        }
    }
}