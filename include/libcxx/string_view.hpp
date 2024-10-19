#pragma once
#include <libcxx/cstring.hpp>

PUBLIC namespace std {
    class string_view
    {
    public:
        string_view( IN const char *__val ) {
            memset( buf, 0, 1024 );
            strcpy( this->buf, __val );
        }
        auto c_str( ) -> char * {
            return buf;
        }
        auto length( ) -> unsigned long {
            return strlen( this->buf );
        }
        auto operator[]( size_t index ) -> char & {
            return this->buf[ index ];
        }

    private:
        char buf[ 1024 ];
    };
}