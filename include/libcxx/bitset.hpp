#pragma once
#include <algorithm>
#include <bit>
#include <cstdint>
#include <expected>
#include <libcxx/cstring.hpp>
#include <utility>
namespace std {

template < std::size_t N >
class bitset {
    enum class error_code {
        ScopeStackoverflow,
        NotFound
    };

public:
    class reference {
        friend bitset< N >;

    private:
        reference( uint64_t *bit, uint64_t pos ) :
            bit_ { bit }, pos_ { pos } {
        }
        ~reference( void ) {
            this->bit_ = nullptr;
        }

    public:
        auto operator=( bool x ) noexcept -> reference & {
            if ( x ) {
                this->bit_[ pos_ / 64 ] |= ( 1ul << this->pos_ % 64 );
            }
            else {
                this->bit_[ pos_ / 64 ] &= ~( 1ul << this->pos_ % 64 );
            }
            return *this;
        }
        operator bool( ) const {
            return this->bit_[ pos_ / 64 ] & ( 1ul << this->pos_ % 64 );
        }

        // ~00001000
        // 11110111 & 00001000 = 00000000
        // ~00000000
        // 11111111 & 00001000 = 00001000

        auto operator~( ) const {
            return this->bit_[ pos_ / 64 ] & ~( 1ul << this->pos_ % 64 );
        }
        auto flip( ) {
            return this->bit_[ pos_ / 64 ] & ~( 1ul << this->pos_ % 64 );
        }

    private:
        uint64_t *bit_;
        uint64_t  pos_;
    };

public:
    explicit bitset( void ) noexcept {
    }
    explicit bitset( std::uint64_t _bitmap[] ) noexcept {
        std::ranges::copy_n( this->bitmap, this->length, _bitmap );
    }
    auto all( ) {
        constexpr auto mask = ~0ull;
        for ( auto &i : this->bitmap ) {
            if ( i != mask ) {
                return false;
            }
        }
        return true;
    }
    auto any( ) {
        for ( auto &i : this->bitmap ) {
            if ( !i ) {
                continue;
            }
            for ( auto j { 0ul }; j < 64; ++i ) {
                if ( i & ( 1ul << j ) ) {
                    return true;
                }
            }
        }
        return false;
    }
    auto none( ) {
        for ( auto &i : this->bitmap ) {
            if ( i ) {
                return false;
            }
        }
        return true;
    }
    auto test( std::size_t pos ) -> reference {
        return { this->bitmap, pos };
    }
    auto operator[]( std::size_t pos ) -> reference {
        return { this->bitmap, pos };
    }
    auto count( ) {
        auto count { 0ul };
        for ( auto &i : this->bitmap ) {
            for ( auto j = 0; j < 64; ++j ) {
                if ( ( 1ul << j ) & i ) {
                    count++;
                }
            }
        }
        return count;
    }
    auto size( ) {
        return N;
    }
    template < bool value = true >
    auto set( std::size_t pos ) -> bitset & {
        if constexpr ( value ) {
            this->bitmap[ pos / 64 ] |= ( 1ul << pos % 64 );
        }
        else {
            this->bitmap[ pos / 64 ] &= ~( 1ul << pos % 64 );
        }
        return *this;
    }
    auto set( ) -> bitset & {
        for ( auto &i : this->bitmap ) {
            i = ~0ull;
        }
        return *this;
    }
    template < bool value = true >
    auto set( std::size_t pos, std::uint64_t size ) -> bitset & {
        if ( !size ) {
            return *this;
        }
        auto &&bitmap_index = pos / 64;
        if constexpr ( value ) {
            if ( 64 - pos % 64 >= size ) {
                bitmap[ bitmap_index ] |= ( ( ( 1ul << ( size - 1 ) ) - 1 ) << pos % 64 ) | ( ( 1ul << ( size - 1 ) ) << pos % 64 );
                return *this;
            }
        }
        else {
            if ( 64 - pos % 64 >= size ) {
                bitmap[ bitmap_index ] &= ~( ( ( ( 1ul << ( size - 1 ) ) - 1 ) << pos % 64 ) | ( ( 1ul << ( size - 1 ) ) << pos % 64 ) );
                return *this;
            }
        }
        uint64_t used_length { };
        if ( auto &&tmp = ( size - ( 64 - pos % 64 ) ); !( tmp % 64 ) ) {
            used_length = tmp / 64;
        }
        else {
            used_length = tmp / 64 + 1;
        }
        auto &&head_mask = ( ( ( 1ul << ( 63 - pos % 64 ) ) - 1 ) << pos % 64 ) | ( ( 1ul << ( 63 - pos % 64 ) ) << pos % 64 );
        auto &&end_mask  = ( ( ( 1ul << ( ( size - ( 64 - pos % 64 ) - 1 ) % 64 ) ) - 1 ) | ( ( 1ul << ( ( size - ( 64 - pos % 64 ) - 1 ) % 64 ) ) ) );
        if constexpr ( value ) {
            bitmap[ bitmap_index ] |= head_mask;
            this->bitmap[ bitmap_index + used_length ] |= end_mask;
        }
        else {
            bitmap[ bitmap_index ] &= ~head_mask;
            this->bitmap[ bitmap_index + used_length ] &= ~end_mask;
        }
        for ( auto _ { bitmap_index + 1 }; _ < bitmap_index + used_length; ++_ ) {
            this->bitmap[ _ ] = ~( (uint64_t)value );
        }
        return *this;
    }
    auto reset( std::size_t pos ) -> bitset & {
        this->bitmap[ pos / 64 ] &= ~( 1ul << pos % 64 );
        return *this;
    }
    auto reset( ) -> bitset & {
        for ( auto &i : this->bitmap ) {
            i = 0ull;
        }
        return *this;
    }
    auto flip( std::size_t pos ) -> bitset & {
        if ( auto &&mask = ( 1ul << pos % 64 ); this->bitmap[ pos / 64 ] & mask ) {
            this->bitmap[ pos / 64 ] &= ~mask;
        }
        else {
            this->bitmap[ pos / 64 ] |= mask;
        }
        return *this;
    }
    auto flip( ) -> bitset & {
        for ( auto &i : this->bitmap ) {
            i = ~i;
        }
        return *this;
    }
    auto operator&=( const bitset &other ) -> bitset & {
        return *this;
    }
    auto operator|=( const bitset &other ) -> bitset & {
        return *this;
    }
    auto operator^=( const bitset &other ) -> bitset & {
        return *this;
    }
    auto operator~( ) const -> bitset {
        std::uint64_t tmp_bitmap[ length ] { bitmap };
        for ( auto &temp : tmp_bitmap ) {
            temp = ~temp;
        }
        return tmp_bitmap;
    }

    template < bool value >
    auto find( std::size_t size = 1 ) -> std::expected< uint64_t, error_code > {
        for ( uint64_t i = 0; i < this->length; ++i ) {
            for ( uint64_t j = 0; j < 64; ++j ) {
                if ( !( this->bitmap[ i ] & ( 1ul << j ) ) ) {
                    if ( 64 - j >= size ) {
                        if ( !( ( this->bitmap[ i ] >> j ) & ( ( 1ul << size ) - 1 ) ) ) {
                            return { i * 64 + j };
                        }
                        continue;
                    }
                    if ( this->bitmap[ i ] >> j ) {
                        continue;
                    }
                    uint64_t used_length { };
                    if ( auto tmp = ( size - ( 64 - j ) ); !( tmp % 64 ) ) {
                        used_length = tmp / 64;
                    }
                    else {
                        used_length = tmp / 64 + 1;
                    }
                    auto success { true };
                    for ( auto &&_ { i + 1 }; _ < used_length + i; ++_ ) {
                        if ( this->bitmap[ _ ] ) {
                            success = false;
                            break;
                        }
                    }
                    if ( !success ) {
                        continue;
                    }
                    if ( this->bitmap[ used_length + i ] & ( ( 1ul << ( ( size - ( 64 - j ) ) % 64 ) ) - 1 ) ) {
                        continue;
                    }
                    return { i * 64 + j };
                }
            }
        }
        return std::unexpected { error_code::NotFound };
    }
    template < bool value >
    auto find_from_high( void ) -> uint64_t {
        for ( int64_t i = this->length - 1; i >= 0; --i ) {
            if constexpr ( value ) {
                if ( this->bitmap[ i ] != ~0ul ) {
                    for ( int64_t j = 63; j >= 0; --j ) {
                        if ( this->bitmap[ i ] & ( 1ul << j ) ) {
                            return i * 64 + j;
                        }
                    }
                }
            }
            else {
                if ( this->bitmap[ i ] != 0ul ) {
                    for ( int64_t j = 63; j >= 0; --j ) {
                        if ( this->bitmap[ i ] & ( 1ul << j ) ) {
                            return i * 64ul + j;
                        }
                    }
                }
            }
        }

        return 0ul;
    }
    template < bool value >
    auto count_from_high( ) -> uint64_t {
        auto number_of_bits = 0ul;
        for ( int64_t i = this->length - 1; i >= 0; --i ) {
            if constexpr ( value ) {
                if ( this->bitmap[ i ] != ~0ul ) {
                    number_of_bits += std::countl_one( this->bitmap[ i ] );
                    return number_of_bits;
                }
            }
            else {
                if ( this->bitmap[ i ] != 0ul ) {
                    number_of_bits += std::countl_zero( this->bitmap[ i ] );
                    return number_of_bits;
                }
            }
            number_of_bits += 64;
        }
        return number_of_bits;
    }
    template < bool value >
    auto count_from_low( ) -> uint64_t {
        auto number_of_bits = 0ul;
        for ( auto i = 0ul; i < this->length; ++i ) {
            if constexpr ( value ) {
                if ( this->bitmap[ i ] != ~0ul ) {
                    number_of_bits += std::countr_one( this->bitmap[ i ] );
                    return number_of_bits;
                }
            }
            else {
                if ( this->bitmap[ i ] != 0ul ) {
                    number_of_bits += std::countr_zero( this->bitmap[ i ] );
                    return number_of_bits;
                }
            }
            number_of_bits += 64;
        }
        return number_of_bits;
    }

private:
    constexpr static auto length = ( N % 64 == 0 ? N / 64 : N / 64 + 1 );
    uint64_t              bitmap[ length ] { };
};
}     // namespace std
