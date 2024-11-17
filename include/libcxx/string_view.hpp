#pragma once
#include <libcxx/cstring.hpp>
PUBLIC namespace std {
    class string_view {
    public:
        string_view( IN const char *__val ) {
            this->buf = __val;
        }
        auto c_str( ) const -> const char * {
            return buf;
        }
        auto length( ) -> unsigned long {
            return strlen( this->buf );
        }
        auto operator[]( size_t index ) const -> char {
            return this->buf[ index ];
        }

    private:
        const char *buf;
    };
}