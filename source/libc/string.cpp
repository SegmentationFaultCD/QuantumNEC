#include <lib/Uefi.hpp>
#include <libc/string.h>

using namespace QuantumNEC;
/**
 * @brief 复制字符串
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
PUBLIC _C_LINK auto strcpy( IN OUT char_t   *Dest,
                            IN CONST char_t *Src )
    -> char_t * {
    auto tmp { Dest };
    for ( auto i { 0ul }; Src[ i ]; ++i )
        Dest[ i ] = Src[ i ];
    return tmp;
}

/**
 * @brief 复制字符串(安全版)
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @param Count 要复制的字符数
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
PUBLIC _C_LINK auto strncpy( IN OUT char_t   *Dest,
                             IN CONST char_t *Src,
                             IN CONST int64_t Count )
    -> char_t * {
    auto tmp { Dest };
    auto n { Count };
    for ( auto i { 0ul }; n-- > 0 && Src[ i ]; ++i )
        Dest[ i ] = Src[ i ];
    return tmp;
}
/**
 * @brief 拼接字符串
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
PUBLIC _C_LINK auto strcat( IN OUT char_t   *Dest,
                            IN CONST char_t *Src )
    -> char_t * {
    auto tmp { Dest };
    while ( *tmp )
        tmp++;
    for ( auto i { 0ul }; Src[ i ]; ++i )
        tmp[ i ] = Src[ i ];
    return Dest;
}
/**
 * @brief 拼接字符串(安全版)
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @param Count 要复制的字符数
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
PUBLIC _C_LINK auto strncat( IN OUT char_t   *Dest,
                             IN CONST char_t *Src,
                             IN CONST int64_t Count )
    -> char_t * {
    auto tmp { Dest };
    auto n { Count };
    for ( auto i { 0ul }; n-- > 0 && Src[ i ]; ++i )
        Dest[ i ] = Src[ i ];
    return tmp;
}
/**
 * @brief 比较字符串
 * @param FirstPart 要比较字符串1
 * @param SecondPart 要比较字符串2
 * @return 如果返回值小于 0，则表示 FirstPart 小于 SecondPart;
 * @return 如果返回值大于 0，则表示 FirstPart 大于 SecondPart;
 * @return 如果返回值等于 0，则表示 FirstPart 等于 SecondPart;
 */
PUBLIC _C_LINK auto strcmp( IN CONST char_t *FirstPart,
                            IN CONST char_t *SecondPart )
    -> int16_t {
    while ( *FirstPart == *SecondPart ) {
        if ( !( *FirstPart ) )
            return 0;
        FirstPart++;
        SecondPart++;
    }
    return *FirstPart - *SecondPart;
}
/**
 * @brief 比较字符串(安全版)
 * @param FirstPart 要比较字符串1
 * @param SecondPart 要比较字符串2
 * @param Count 要比较的字符数
 * @return 如果返回值小于 0，则表示 FirstPart 小于 SecondPart;
 * @return 如果返回值大于 0，则表示 FirstPart 大于 SecondPart;
 * @return 如果返回值等于 0，则表示 FirstPart 等于 SecondPart;
 */
PUBLIC _C_LINK auto strncmp( IN CONST char_t *FirstPart,
                             IN CONST char_t *SecondPart,
                             IN CONST int64_t Count )
    -> int16_t {
    auto count { Count };
    if ( !count )
        return 0;
    while ( --count && *FirstPart
            && *FirstPart == *SecondPart )     // 把第count个字符留到最后处理。
    {
        FirstPart++;
        SecondPart++;
    }
    return *reinterpret_cast< char_t * >(
               const_cast< char_t * >( FirstPart ) )
           - *reinterpret_cast< char_t * >(
               const_cast< char_t * >( SecondPart ) );
}
/**
 * @brief 测量字符串
 * @param String 要测量的字符串
 * @return 返回字符串长度(不包含\0)
 */
PUBLIC _C_LINK auto strlen( IN CONST char_t *String ) -> size_t {
    auto strSize { 0ULL };
    auto tmp { String };
    while ( *tmp ) {
        ++tmp;
        ++strSize;
    }
    return strSize;
}
/**
 * @brief 查找字符在指定字符串中第一次出现的位置
 * @param dest 目标字符串
 * @param ch 要查的字符
 * @return 开始dest指向的地址
 */
PUBLIC _C_LINK auto strchr( IN CONST char_t *dest, IN int32_t ch ) -> char_t * {
    if ( !dest ) {
        return NULL;
    }
    auto src { const_cast< char_t * >( dest ) };
    while ( *src++ ) {
        if ( *src == ch ) {
            return src;
        }
    }
    return NULL;
}
/**
 * @brief 查找字符在指定字符串中从右面开始的第一次出现的位置
 * @param dest 目标字符串
 * @param ch 要查的字符
 * @return 开始dest指向的地址
 */
PUBLIC _C_LINK auto strrchr( IN CONST char_t *dest, IN int32_t ch ) -> char_t * {
    char_t *_src_one { strchr( dest, ch ) };
    char_t *_src_two { };
    while ( _src_one ) {
        _src_two = strchr( _src_one + 1, ch );
        if ( _src_two ) {
            _src_one = _src_two;
        }
        else
            return _src_one;     // 当这个没有匹配到时即返回上一步的位置
    }
    return NULL;     // 全部未匹配
}
/**
 * @brief 在字符串中查找第一次出现另一个字符串的位置
 * @param haystack 要被检索的字符串
 * @param needle 要搜索的字符串
 * @return 返回在haystack中第一次出现needle字符串的地址，如果未找到则返回NULL
 */
PUBLIC _C_LINK auto strstr( IN char_t       *haystack,
                            IN CONST char_t *needle )
    -> char_t * {
    if ( *needle ) {
        while ( *haystack ) {
            for ( int32_t n { }; *( haystack + n ) == *( needle + n );
                  n++ ) {
                if ( !*( needle + n + 1 ) )
                    return haystack;
            }
            haystack++;
        }
        return NULL;
    }
    else
        return haystack;
}
/**
 * @brief 拷贝内存内容
 * @param dest 目标内存地址
 * @param src 源内存地址
 * @param size 字节数
 * @return 返回指向dest的指针
 */
PUBLIC _C_LINK auto memcpy( IN OUT VOID   *dest,
                            IN CONST VOID *src,
                            IN uint64_t    size )
    -> VOID * {
    auto ret { dest };
    /*
     * 从较低地址复制到较高地址
     */
    while ( size-- ) {
        *reinterpret_cast< char_t * >( dest ) = *reinterpret_cast< char_t * >( const_cast< VOID * >( src ) );
        dest                                  = reinterpret_cast< char_t                                  *>( dest ) + 1;
        src                                   = reinterpret_cast< char_t                                   *>( const_cast< VOID                                   *>( src ) ) + 1;
    }
    return reinterpret_cast< VOID * >( ret );
}

/**
 * @brief 查找内存内容
 * @param buf 缓冲区
 * @param ch 要查找的字符
 * @param count 往前字节数
 * @return 当未找到，返回NULL;
 * @return 当找到，返回指向ch的指针
 */
PUBLIC _C_LINK auto memchr( IN OUT VOID *buf,
                            IN int8_t value, IN size_t size )
    -> VOID * {
    if ( !buf )
        return NULL;
    auto p { reinterpret_cast< char_t * >( buf ) };
    while ( size-- ) {
        if ( *p != value ) {
            p++;
        }
        else
            return reinterpret_cast< VOID * >( p );
    }
    return NULL;
}

/**
 * @brief 比较内存中缓冲区的大小
 * @param buf1 要比较的缓冲区1
 * @param buf2 要比较的缓冲区2
 * @param size 两个缓冲区的前size个字节
 * @return 当buf1 < buf2时，返回值 < 0;
 * @return 当buf1 = buf2时，返回值 = 0;
 * @return 当buf1 > buf2时，返回值 > 0
 */
PUBLIC _C_LINK auto memcmp( IN VOID          *buf1,
                            IN VOID          *buf2,
                            IN CONST uint64_t size )
    -> int16_t {
    auto pdest1 { (char *)buf1 }, pdest2 { (char *)buf2 };
    auto n { size };
    while ( n-- > 0 ) {
        if ( *pdest1 != *pdest2 )
            return *pdest1 - *pdest2;
        pdest1++, pdest2++;
    }
    return 0;
}

/**
 * @brief 拷贝内存内容(相比于memcpy效率更低但是更安全)
 * @param dest 目标内存地址
 * @param src 源内存地址
 * @param size 字节数
 * @return 返回指向dest的指针
 */
PUBLIC _C_LINK auto memmove( IN OUT VOID       *dest,
                             IN OUT CONST VOID *src,
                             IN size_t          size )
    -> VOID * {
    auto __src { const_cast< VOID * >( src ) };
    auto ret { dest };
    if ( dest <= src || reinterpret_cast< char_t * >( dest ) >= ( reinterpret_cast< char_t * >( __src ) + size ) ) {
        /* 非重叠缓冲区
         * 从较低的地址复制到较高的地址 */
        while ( size-- ) {
            *reinterpret_cast< char_t * >( dest ) = *reinterpret_cast< char_t * >( __src );
            dest                                  = reinterpret_cast< char_t                                  *>( dest ) + 1;
            __src                                 = reinterpret_cast< char_t                                 *>( __src ) + 1;
        }
    }
    else {
        /* 重叠缓冲区
         * 从较高的地址复制到较低的地址 */
        dest  = reinterpret_cast< char_t  *>( dest ) + size - 1;
        __src = reinterpret_cast< char_t * >( __src ) + size - 1;
        while ( size-- ) {
            *reinterpret_cast< char_t * >( dest ) = *reinterpret_cast< char_t * >( __src );
            dest                                  = reinterpret_cast< char_t                                  *>( dest ) - 1;
            __src                                 = reinterpret_cast< char_t                                 *>( __src ) - 1;
        }
    }
    return ret;
}

/**
 * @brief 设置内存内容
 * @param dest 所指向的内存单元
 * @param value 要替换的数
 * @param size dest前几个的字节
 */
PUBLIC _C_LINK auto memset( IN OUT CONST VOID *dest,
                            IN CONST uint8_t   value,
                            IN CONST uint64_t  size )
    -> VOID * {
    unsigned char *ptr   = (unsigned char *)dest;
    unsigned char  val   = (unsigned char)value;
    auto           count = size;
    // 处理未对齐的字节
    while ( count > 0 && ( (size_t)ptr & ( sizeof( size_t ) - 1 ) ) ) {
        *ptr++ = val;
        count--;
    }

    // 如果还有足够的字节要处理
    if ( count >= sizeof( size_t ) ) {
        size_t  val_word;
        size_t *word_ptr;

        // 构造填充字
        val_word = ( val << 24 ) | ( val << 16 ) | ( val << 8 ) | val;
#if SIZE_MAX > 0xffffffff
        val_word = ( val_word << 32 ) | val_word;
#endif

        // 按字填充
        word_ptr = (size_t *)ptr;
        while ( count >= sizeof( size_t ) ) {
            *word_ptr++ = val_word;
            count -= sizeof( size_t );
        }

        ptr = (unsigned char *)word_ptr;
    }

    // 处理剩余字节
    while ( count-- ) {
        *ptr++ = val;
    }

    return (VOID *)dest;
}

/**
 * @brief 转换数字为字符串（数字有符号）
 * @param num       要转换的数字(有符号)
 * @param str     转换后的字符串的存储地址
 * @param base    进制,最高支持36进制
 */
PUBLIC _C_LINK auto itoa( IN int64_t num, IN char_t *str, IN uint32_t base ) -> VOID {
    auto digits { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    auto i { 0 };             /* 作为下标来索引 */
    auto is_negative { num }; /* 是否是负的？ */
    if ( num < 0 )            /* 如果是负数 */
    {
        num = -num; /* 先转换为正数 */
    }
    /* 转换为字符串,不过是倒过来的 */
    do {
        str[ i ] = digits[ num % base ];
        i++;
        num = num / base;
    } while ( num > 0 );
    /* 是负数,就加上负号 */
    if ( is_negative < 0 ) {
        str[ i ] = '-';
        i++;
    }
    str[ i ] = '\0'; /* 加上字符串结尾 */
    auto  *p { str };
    auto  *q { str };
    char_t tmp { };
    while ( *q != '\0' ) {
        q++;
    }
    q--;
    /* 把字符串倒过来 */
    while ( q > p ) {
        tmp = *p;
        *p  = *q;
        p++;
        *q = tmp;
        q--;
    }
}
/**
 * @brief 转换数字为字符串（数字无符号）
 * @param num       要转换的数字(无符号)
 * @param str     转换后的字符串的存储地址
 * @param base    进制,最高支持36进制
 */
PUBLIC _C_LINK auto utoa( IN uint64_t num, IN char_t *str, IN uint32_t base ) -> VOID {
    auto digits { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    auto i { 0 }; /* 作为下标来索引 */
    /* 转换为字符串,不过是倒过来的 */
    do {
        str[ i ] = digits[ num % base ];
        i++;
        num = num / base;
    } while ( num > 0 );
    str[ i ] = '\0'; /* 加上字符串结尾 */
    auto   p { str };
    auto   q { str };
    char_t tmp { };
    while ( *q != '\0' ) {
        q++;
    }
    q--;
    /* 把字符串倒过来 */
    while ( q > p ) {
        tmp = *p;
        *p  = *q;
        p++;
        *q = tmp;
        q--;
    }
    return;
}
