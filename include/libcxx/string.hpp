#pragma once
#include <libcxx/cstring.hpp>
#include <libcxx/memory.hpp>
namespace std {
template < class CharT >
class char_traits;
template <>
class char_traits< char > {
};

template < typename _CharT,
           typename _Traits = char_traits< _CharT >,
           typename _Alloc  = allocator< _CharT > >
class basic_string {
};
}     // namespace std
// PUBLIC namespace QuantumNEC::Lib {
//     template < typename _CharT,
//                typename _Traits = char_t,
//                typename _Alloc = char_t >
//     class basic_string;
//     using string = basic_string< char_t >;
//     template <>
//     class basic_string< char_t >
//     {     // 字符串
//         char_t *__str { };

//         // 字符串长度
//         size_t __str__length { };

//     public:
//         explicit basic_string( IN basic_string< char_t > &&_string ) noexcept {
//             // 移动就把他所属权转移
//             this->__str__length = _string.__str__length;
//             this->__str = _string.__str;
//             _string.__str = NULL;
//         }
//         explicit basic_string( VOID ) noexcept {
//         }
//         explicit basic_string( IN char_t *_string ) noexcept {
//             this->__str__length = strlen( _string ) + 1;
//             this->__str = new char_t[ this->__str__length ];
//             strncpy( this->__str, _string, this->__str__length );
//         }
//         explicit basic_string( IN basic_string< char_t > &_string ) noexcept {
//             // 不是移动就复制
//             this->__str__length = _string.__str__length;
//             this->__str = new char_t[ this->__str__length ];
//             strncpy( this->__str, _string.__str, this->__str__length );
//         }

//         virtual ~basic_string( VOID ) noexcept {
//             if ( this->__str ) {
//                 // 析构str
//                 delete[] this->__str;
//                 this->__str = NULL;
//             }
//         }

//         auto operator=( IN basic_string< char_t > &_string ) -> basic_string< char_t > & {
//             this->__str__length = _string.__str__length;
//             this->__str = new char_t[ this->__str__length ];
//             strncpy( this->__str, _string.__str, this->__str__length );
//             return *this;
//         }

//     public:
//         friend auto operator+( IN basic_string &, IN CONST basic_string & ) -> basic_string;
//         friend auto operator+( IN CONST char_t *, IN basic_string & ) -> basic_string;
//         friend auto operator+( IN basic_string &, IN CONST char_t * ) -> basic_string;
//         friend auto operator+( IN CONST char_t, IN basic_string & ) -> basic_string;
//         friend auto operator+( IN basic_string &, IN CONST char_t ) -> basic_string;
//         friend auto operator+=( IN basic_string &, IN CONST basic_string & ) -> CONST basic_string &;
//         friend auto operator+=( IN basic_string &, IN char_t * ) -> CONST basic_string &;
//         friend auto operator+=( IN basic_string &, IN CONST char_t ) -> CONST basic_string &;

//     public:
//         constexpr auto operator=( IN basic_string &&_string ) -> CONST basic_string & {
//             // 移动就把他所属权转移
//             this->__str__length = _string.__str__length;
//             this->__str = _string.__str;
//             _string.__str = NULL;
//             return *this;
//         }

//         constexpr auto operator==( IN CONST char_t *_string ) -> BOOL {
//             return !strcmp( _string, this->__str ) ? TRUE : FALSE;
//         }
//         constexpr auto operator==( IN basic_string &_string ) -> BOOL {
//             return *this == _string.__str;
//         }

//     public:
//         constexpr auto size( VOID ) CONST {
//             return __str__length;
//         }
//         constexpr auto length( VOID ) CONST {
//             return __str__length;
//         }
//         constexpr auto c_str( VOID ) -> char_t * {
//             return this->__str;
//         }
//         constexpr auto operator[]( IN size_t size ) const -> CONST char_t & {
//             if ( size > this->__str__length )
//                 return this->__str[ this->__str__length ];
//             return this->__str[ size ];
//         }
//     };

//     auto operator+( IN basic_string< char_t > &_string1, IN CONST basic_string< char_t > &_string2 )->basic_string< char_t > {
//         auto frame = new char_t[ _string1.__str__length - 1 + _string2.__str__length ];
//         strncat( frame, _string1.__str, _string1.__str__length - 1 );
//         strncat( _string1.__str, _string2.__str, _string2.__str__length );
//         return basic_string< char_t > { frame };
//     }

//     auto operator+( IN CONST char_t *_string1, IN basic_string< char_t > &_string2 )->basic_string< char_t > {
//         auto frame = new char_t[ strlen( _string1 ) + _string2.__str__length ];
//         strncat( frame, _string1, strlen( _string1 ) );
//         strncat( frame, _string2.__str, _string2.__str__length );
//         return basic_string< char_t > { frame };
//     }
//     auto operator+( IN basic_string< char_t > &_string1, IN CONST char_t *_string2 )->basic_string< char_t > {
//         auto frame = new char_t[ _string1.__str__length - 1 + strlen( _string2 ) + 1 ];
//         strncat( frame, _string1.__str, _string1.__str__length - 1 );
//         strncat( frame, _string2, strlen( _string2 ) + 1 );
//         return basic_string< char_t > { frame };
//     }
//     auto operator+( IN basic_string< char_t > &_string, IN CONST char_t ch )->basic_string< char_t > {
//         char_t buf[] { ch, '\0' };
//         auto frame = new char_t[ _string.__str__length + 1 ];
//         strncat( frame, _string.__str, _string.__str__length - 1 );
//         strncat( frame, buf, 2 );
//         return basic_string< char_t > { frame };
//     }
//     auto operator+( IN CONST char_t ch, IN basic_string< char_t > &_string )->basic_string< char_t > {
//         char_t buf[] { ch };
//         auto frame = new char_t[ 1 + _string.__str__length ];
//         strncat( frame, buf, 1 );
//         strncat( frame, _string.__str, _string.__str__length );
//         return basic_string< char_t > { frame };
//     }
//     auto operator+=( IN basic_string< char_t > &_string1, IN CONST basic_string< char_t > &_string2 )->CONST basic_string< char_t > & {
//         _string1 = _string1 + _string2;
//         return _string1;
//     }
//     auto operator+=( IN basic_string< char_t > &_string1, IN CONST char_t *_string2 )->CONST basic_string< char_t > & {
//         _string1 = _string1 + _string2;
//         return _string1;
//     }

//     auto operator+=( IN basic_string< char_t > &_string, IN CONST char_t ch )->CONST basic_string< char_t > & {
//         _string = _string + ch;
//         return _string;
//     }
// }