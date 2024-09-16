#pragma once
#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC constexpr CONST auto NO_TASK { 0x1919810 };
    PUBLIC constexpr CONST auto ANY { 0x018919 };
    PUBLIC constexpr CONST auto INTERRUPT { 0x114514 };
    PUBLIC class Message : public message_t
    {
    public:
        explicit Message( VOID ) noexcept = default;
        ~Message( VOID ) noexcept = default;

    public:
        /**
         * @brief 发送消息
         */
        auto send( IN uint64_t destination ) -> SyscallStatus;
        /**
         * @brief 接收消息
         */
        auto receive( IN uint64_t source ) -> SyscallStatus;
    };
}