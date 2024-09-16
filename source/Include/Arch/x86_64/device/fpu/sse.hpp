#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    class Sse
    {
    public:
        struct alignas( 16 ) SSE
        {
            uint8_t reserved5[ 512 ];
            explicit SSE( VOID ) noexcept = default;
            /**
             * @brief 检查fpu是否完好
             * @return 完好就是TRUE，不完好就是FALSE
             */
            auto check_fpu( VOID ) -> BOOL;
            /**
             * @brief 读取fpu
             * @param task 当前进程
             */
            auto read_sse( VOID ) -> VOID;
            /**
             * @brief 设置fpu
             * @param task 当前进程
             */
            auto load_sse( VOID ) -> VOID;
        };

    public:
        explicit Sse( VOID ) noexcept;
        virtual ~Sse( VOID ) noexcept = default;
    };
}
