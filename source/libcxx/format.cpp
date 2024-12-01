#include <libcxx/format.hpp>
namespace std {
auto formatter< char >::format( char value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    _buffer[ 0 ] = value;
    _buffer[ 1 ] = '\0';
    return _buffer;
}
auto formatter< char && >::format( char &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    _buffer[ 0 ] = value;
    _buffer[ 1 ] = '\0';
    return _buffer;
}
auto formatter< char * >::format( char *value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    strcat( _buffer, value );
    return _buffer;
}
auto formatter< const char * >::format( const char *value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    strcat( _buffer, value );
    return _buffer;
}

auto formatter< std::int32_t && >::format( std::int32_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    itoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::int16_t && >::format( std::int16_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    itoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::int64_t && >::format( std::int64_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    itoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::uint32_t && >::format( std::uint32_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::uint16_t && >::format( std::uint16_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::uint64_t && >::format( std::uint64_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< void * >::format( void *value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( (std::uint64_t)value, _buffer, 16 );
    return _buffer;
}
auto formatter< std::uint8_t >::format( std::uint8_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( (std::uint64_t)value, _buffer, number_format );
    return _buffer;
}
auto formatter< std::uint8_t && >::format( std::uint8_t &&value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( (std::uint64_t)value, _buffer, number_format );
    return _buffer;
}
auto formatter< double && >::format( double &&__val ) -> char * {
    static char   bufint[ 512 ] { }, buffloat[ 512 ] { };
    char         *buf_int { bufint }, *buf_float { buffloat };
    std::uint64_t integerPart { std::uint64_t( __val ) };
    double        floatPart { __val - std::uint64_t( __val ) };
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
auto formatter< std::int32_t >::format( std::int32_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    itoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::int16_t >::format( std::int16_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    itoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::int64_t >::format( std::int64_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    itoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::uint32_t >::format( std::uint32_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::uint16_t >::format( std::uint16_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< std::uint64_t >::format( std::uint64_t value ) -> char * {
    memset( _buffer, 0, strlen( _buffer ) );
    utoa( value, _buffer, number_format );
    return _buffer;
}

auto formatter< double >::format( double __val ) -> char * {
    static char   bufint[ 512 ] { }, buffloat[ 512 ] { };
    char         *buf_int { bufint }, *buf_float { buffloat };
    std::uint64_t integerPart { std::uint64_t( __val ) };
    double        floatPart { __val - std::uint64_t( __val ) };
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
}     // namespace std