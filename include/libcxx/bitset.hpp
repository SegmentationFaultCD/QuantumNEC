#pragma once
#include <cstdint>
#include <libcxx/expected.hpp>
#include <utility>
PUBLIC namespace std {
    enum class bitset_error_code {
        ScopeStackoverflow,
        NotFound
    };
    template < std::size_t N >
    class bitset {
    public:
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
        explicit bitset( VOID ) noexcept {
        }
        explicit bitset( std::uint64_t _bitmap[] ) noexcept {
            for ( auto i { 0ul }; i < length; ++i ) {
                this->bitmap[ i ] = _bitmap[ i ];
            }
        }
        auto all( ) {
            constexpr auto mask = ~0ull;
            for ( auto i { 0ul }; i < length; ++i ) {
                if ( ( this->bitmap[ i ] != mask ) ) {
                    return false;
                }
            }
            return true;
        }
        auto any( ) {
            for ( auto i { 0ul }; i < length; ++i ) {
                if ( !bitmap[ i ] ) {
                    continue;
                }
                for ( auto j { 0ul }; j < 64; ++i ) {
                    if ( this->bitmap[ i ] & ( 1ul << j ) ) {
                        return true;
                    }
                }
            }
            return false;
        }
        auto none( ) {
            for ( auto i { 0ul }; i < length; ++i ) {
                if ( this->bitmap[ i ] ) {
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
            for ( auto i { 0ul }; i < length; ++i ) {
                for ( auto j { 0ul }; j < 64ul; ++j ) {
                    if ( this->bitmap[ i ] & ( 1ul << j ) ) {
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
            for ( auto i { 0ul }; i < length; ++i ) {
                this->bitmap[ i ] = ~( 0 );
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
                if constexpr ( value ) {
                    this->bitmap[ _ ] = ~0ul;
                }
                else {
                    this->bitmap[ _ ] = 0ul;
                }
            }
            return *this;
        }
        auto reset( std::size_t pos ) -> bitset & {
            this->bitmap[ pos / 64 ] &= ~( 1ul << pos % 64 );
            return *this;
        }
        auto reset( ) -> bitset & {
            for ( auto i { 0ul }; i < length; ++i ) {
                this->bitmap[ i ] = 0;
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
            for ( auto i { 0ul }; i < length; ++i ) {
                this->bitmap[ i ] = ~this->bitmap[ i ];
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
            for ( std::uint64_t i { }; i < length; ++i ) {
                tmp_bitmap[ i ] = ~tmp_bitmap[ i ];
            }
            return tmp_bitmap;
        }

        auto allocate( std::size_t size ) -> std::expected< uint64_t, bitset_error_code > {
            if ( !size || size > N ) {
                return std::unexpected { bitset_error_code::ScopeStackoverflow };
            }
            if ( auto pos_in_bitmap = this->find< false >( size ); pos_in_bitmap.has_value( ) ) {
                this->set< true >( pos_in_bitmap.value( ), size );
                return pos_in_bitmap.value( );
            }
            return std::unexpected { bitset_error_code::NotFound };
        }
        auto free( std::size_t pos, std::size_t size = 1 ) {
            this->set< false >( pos, size );
        }
        template < bool value >
        auto find( std::size_t size = 1 ) -> std::expected< uint64_t, bitset_error_code > {
            for ( uint64_t i { }; i < this->length; ++i ) {
                for ( uint64_t j { }; j < 64; ++j ) {
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
                        bool success { true };
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
            return std::unexpected { bitset_error_code::NotFound };
        }

    private:
        inline static constexpr auto length = ( N % 64 == 0 ? N / 64 : N / 64 + 1 );
        uint64_t                     bitmap[ length ] { };
    };
}
