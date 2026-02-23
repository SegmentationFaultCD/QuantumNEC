#pragma once
#include <cstdint>

namespace Task {
// 4K / 2^5 = 2^12 / 2^5 = 2^7 = 128
template < typename T >
struct alignas( 32 ) Message {
    std::uint64_t node;
    T *package_address;
    std::uint64_t length;
    std::uint64_t lock;
};

}     // namespace Task