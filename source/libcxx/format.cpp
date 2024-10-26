#include <libcxx/format.hpp>
PUBLIC namespace std {
    auto formatter< char >::format( const char &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        _buffer[ 0 ] = value;
        _buffer[ 1 ] = '\0';
        return _buffer;
    }

    auto formatter< char * >::format( char *&value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        strcat( _buffer, value );
        return _buffer;
    }

    auto formatter< const char * >::format( const char *&value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        strcat( _buffer, value );
        return _buffer;
    }

    auto formatter< std::int32_t >::format( const std::int32_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        itoa( value, _buffer, number_format );
        return _buffer;
    }

    auto formatter< std::int16_t >::format( const std::int16_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        itoa( value, _buffer, number_format );
        return _buffer;
    }

    auto formatter< std::int64_t >::format( const std::int64_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        itoa( value, _buffer, number_format );
        return _buffer;
    }

    auto formatter< std::uint32_t >::format( const std::uint32_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        utoa( value, _buffer, number_format );
        return _buffer;
    }

    auto formatter< std::uint16_t >::format( const std::uint16_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        utoa( value, _buffer, number_format );
        return _buffer;
    }

    auto formatter< std::uint64_t >::format( const std::uint64_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        utoa( value, _buffer, number_format );
        return _buffer;
    }

    auto formatter< VOID * >::format( VOID * value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        utoa( (std::uint64_t)value, _buffer, 16 );
        return _buffer;
    }

    auto formatter< std::uint8_t >::format( const std::uint8_t &value ) -> char * {
        memset( _buffer, 0, strlen( _buffer ) );
        utoa( (std::uint64_t)value, _buffer, number_format );
        return _buffer;
    }
    auto formatter< double64_t >::format( const double64_t &__val ) -> char * {
        STATIC char_t bufint[ 512 ] { }, buffloat[ 512 ] { };
        char_t *buf_int { bufint }, *buf_float { buffloat };
        std::uint64_t integerPart { std::uint64_t( __val ) };
        double64_t floatPart { __val - std::uint64_t( __val ) };
        do {
            buf_int = char( integerPart % 10 + '0' ) + buf_int;
            integerPart /= 10;
        } while ( integerPart );
        do {
            floatPart *= 10;
        } while ( floatPart - std::uint64_t( floatPart ) >= 1e-6 );
        std::uint64_t floatPart__Val { std::uint64_t( floatPart ) };
        do {
            buf_float = char( floatPart__Val % 10 + '0' ) + buf_float;
            floatPart__Val /= 10;
        } while ( floatPart__Val );
        strcat( buf_int, "." );
        strcat( buf_int, buf_float );
        return buf_int;
    }
}