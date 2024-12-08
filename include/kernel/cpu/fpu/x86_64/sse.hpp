#pragma once
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Sse {
public:
    struct _aligned( 16 ) SSE {
        uint8_t fxsave_region[ 512 ] { };
        explicit SSE( void ) noexcept = default;
        /**
         * @brief 检查fpu是否完好
         * @return 完好就是TRUE，不完好就是FALSE
         */
        auto check_fpu( void ) -> bool;
        /**
         * @brief 读取fpu
         * @param task 当前进程
         */
        auto save( void ) -> void;
        /**
         * @brief 设置fpu
         * @param task 当前进程
         */
        auto load( void ) -> void;
    };

public:
    static auto activate( void ) noexcept -> void;

public:
    explicit Sse( void ) noexcept;
    virtual ~Sse( void ) noexcept = default;
};
}     // namespace QuantumNEC::Kernel::x86_64
