#pragma once
#include <cstdint>
namespace std {
extern "C" {
/**
 * @brief 复制字符串
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
auto strcpy( char       *Dest,
             const char *Src ) -> char *;

/**
 * @brief 复制字符串(安全版)
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @param Count 要复制的字符数
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
auto strncpy( char       *Dest,
              const char *Src,
              int64_t     Count )
    -> char *;
/**
 * @brief 拼接字符串
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
auto strcat( char       *Dest,
             const char *Src )
    -> char *;

/**
 * @brief 拼接字符串(安全版)
 * @param Dest 要修改的字符串
 * @param Src 修改Dest的字符串
 * @param Count 要复制的字符数
 * @return 返回一个指向最终的目标字符串Dest的指针
 */
auto strncat( char       *Dest,
              const char *Src,
              int64_t     Count )
    -> char *;
/**
 * @brief 比较字符串
 * @param FirstPart 要比较字符串1
 * @param SecondPart 要比较字符串2
 * @return 如果返回值小于 0，则表示 FirstPart 小于 SecondPart;
 * @return 如果返回值大于 0，则表示 FirstPart 大于 SecondPart;
 * @return 如果返回值等于 0，则表示 FirstPart 等于 SecondPart;
 */
auto strcmp( char       *FirstPart,
             const char *SecondPart )
    -> int16_t;
/**
 * @brief 比较字符串(安全版)
 * @param FirstPart 要比较字符串1
 * @param SecondPart 要比较字符串2
 * @param Count 要比较的字符数
 * @return 如果返回值小于 0，则表示 FirstPart 小于 SecondPart;
 * @return 如果返回值大于 0，则表示 FirstPart 大于 SecondPart;
 * @return 如果返回值等于 0，则表示 FirstPart 等于 SecondPart;
 */
auto strncmp( const char   *FirstPart,
              const char   *SecondPart,
              const int64_t Count )
    -> int16_t;
/**
 * @brief 测量字符串
 * @param String 要测量的字符串
 * @return 返回字符串长度(不包含\0)
 */
auto strlen( const char *string ) -> uint64_t;
/**
 * @brief 查找字符在指定字符串中第一次出现的位置
 * @param dest 目标字符串
 * @param ch 要查的字符
 * @return 开始dest指向的地址
 */
auto strchr( const char *dest, int32_t ch ) -> char *;
/**
 * @brief 查找字符在指定字符串中从右面开始的第一次出现的位置
 * @param dest 目标字符串
 * @param ch 要查的字符
 * @return 开始dest指向的地址
 */
auto strrchr( const char *dest, int32_t ch ) -> char *;
/**
 * @brief 在字符串中查找第一次出现另一个字符串的位置
 * @param haystack 要被检索的字符串
 * @param needle 要搜索的字符串
 * @return 返回在haystack中第一次出现needle字符串的地址，如果未找到则返回NULL
 */
auto strstr( char       *haystack,
             const char *needle )
    -> char *;
/**
 * @brief 拷贝内存内容
 * @param dest 目标内存地址
 * @param src 源内存地址
 * @param size 字节数
 * @return 返回指向dest的指针
 */
auto memcpy( void    *dest,
             void    *src,
             uint64_t size )
    -> void *;

/**
 * @brief 查找内存内容
 * @param buf 缓冲区
 * @param ch 要查找的字符
 * @param count 往前字节数
 * @return 当未找到，返回NULL;
 * @return 当找到，返回指向ch的指针
 */
auto memchr( void    *buf,
             int8_t   value,
             uint64_t size )
    -> void *;

/**
 * @brief 比较内存中缓冲区的大小
 * @param buf1 要比较的缓冲区1
 * @param buf2 要比较的缓冲区2
 * @param size 两个缓冲区的前size个字节
 * @return 当buf1 < buf2时，返回值 < 0;
 * @return 当buf1 = buf2时，返回值 = 0;
 * @return 当buf1 > buf2时，返回值 > 0
 */
auto memcmp( void    *buf1,
             void    *buf2,
             uint64_t size )
    -> int16_t;

/**
 * @brief 拷贝内存内容(相比于memcpy效率更低但是更安全)
 * @param dest 目标内存地址
 * @param src 源内存地址
 * @param size 字节数
 * @return 返回指向dest的指针
 */
auto memmove( void    *dest,
              void    *src,
              uint64_t size )
    -> void *;

/**
 * @brief 设置内存内容
 * @param dest 所指向的内存单元
 * @param value 要替换的数
 * @param size dest前几个的字节
 */
auto memset( void    *dest,
             uint8_t  value,
             uint64_t size )
    -> void *;

/**
 * @brief 转换数字为字符串（数字有符号）
 * @param num       要转换的数字(有符号)
 * @param str     转换后的字符串的存储地址
 * @param base    进制,最高支持36进制
 */
auto itoa( int64_t num, const char *str, uint32_t base ) -> void;
/**
 * @brief 转换数字为字符串（数字无符号）
 * @param num       要转换的数字(无符号)
 * @param str     转换后的字符串的存储地址
 * @param base    进制,最高支持36进制
 */
auto utoa( uint64_t num, const char *str, uint32_t base ) -> void;
}
}     // namespace std