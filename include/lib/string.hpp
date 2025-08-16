#pragma once
#include <cstdint>
#include <cstring>
namespace Library {

extern "C" {
/**
 * @brief 转换数字为字符串（数字有符号）
 * @param num       要转换的数字(有符号)
 * @param str     转换后的字符串的存储地址
 * @param base    进制,最高支持36进制
 */
auto itoa( std::int64_t num, char *str, std::uint32_t base ) -> void;
/**
 * @brief 转换数字为字符串（数字无符号）
 * @param num       要转换的数字(无符号)
 * @param str     转换后的字符串的存储地址
 * @param base    进制,最高支持36进制
 */
auto utoa( std::uint64_t num, char *str, std::uint32_t base ) -> void;
}
}     // namespace Library