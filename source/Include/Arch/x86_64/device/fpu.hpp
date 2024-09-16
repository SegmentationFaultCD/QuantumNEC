#pragma once
#include <Lib/Base/deflib.hpp>
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class FPU
    {
    public:
        struct FPUFrame
        {
            Lib::Types::uint16_t control;
            Lib::Types::uint16_t reserved1;
            Lib::Types::uint16_t status;
            Lib::Types::uint16_t reserved2;
            Lib::Types::uint16_t queue;
            Lib::Types::uint16_t reserved3;
            Lib::Types::uint32_t fip;
            Lib::Types::uint16_t fps;
            Lib::Types::uint16_t fop;
            Lib::Types::uint32_t fdp;
            Lib::Types::uint16_t fds;
            Lib::Types::uint16_t reserved4;
            Lib::Types::uint8_t registers[ 80 ];
            /**
             * @brief 刷新FPU
             */
            auto flush_fpu( VOID ) -> VOID;
            /**
             * @brief 检查fpu是否完好
             * @return 完好就是TRUE，不完好就是FALSE
             */
            auto check_fpu( VOID ) -> Lib::Types::BOOL;
            /**
             * @brief 开启FPU
             * @param task 当前进程
             */
            auto enable_fpu( IN Lib::Types::Ptr< VOID > task ) -> VOID;
            /**
             * @brief 关闭FPU
             * @param task 当前进程
             */
            auto disable_fpu( IN Lib::Types::Ptr< VOID > task ) -> VOID;
        } _packed;

    public:
        explicit FPU( VOID ) noexcept;
        ~FPU( VOID ) noexcept = default;

    public:
    };
}
