#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Lib/STL/list>
#include <Arch/Arch.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto NO_TASK { 0x1919810 };
    PUBLIC constexpr CONST auto ANY { 0x018919 };
    PUBLIC constexpr CONST auto INTERRUPT { 0x114514 };
    PUBLIC class Message
    {
    private:
        union {
            struct
            {
                Lib::Types::uint32_t i1;
                Lib::Types::uint32_t i2;
                Lib::Types::uint32_t i3;
                Lib::Types::uint32_t i4;
            } volatile msg1;
            struct
            {
                Lib::Types::Ptr< VOID > p1;
                Lib::Types::Ptr< VOID > p2;
                Lib::Types::Ptr< VOID > p3;
                Lib::Types::Ptr< VOID > p4;
            } volatile msg2;
            struct
            {
                Lib::Types::uint32_t i1;
                Lib::Types::uint32_t i2;
                Lib::Types::uint32_t i3;
                Lib::Types::uint32_t i4;
                Lib::Types::uint64_t l1;
                Lib::Types::uint64_t l2;
                Lib::Types::Ptr< VOID > p1;
                Lib::Types::Ptr< VOID > p2;
            } volatile msg3;
        } message;

    public:
        Lib::Types::uint64_t source;          /* 发出这个消息的进程pid */
        Lib::Types::uint32_t type;            /* 消息类型 */
        Lib::Types::uint64_t send_to;         /* 记录进程想要向谁发送消息 */
        Lib::Types::uint64_t receive_from;    /* 记录进程想要从谁获取消息 */
        Lib::Types::uint64_t has_int_message; /* 有来自中断的消息,这个结构被中断处理程序置为1 */
        Lib::STL::ListTable sender_list;      /* 如果有进程向这个进程发送消息, 但本进程没有要接收消息,那么发送信息的进程将自己的通任务队列加入这个队列 */

    public:
        explicit Message( VOID ) noexcept = default;
        virtual ~Message( VOID ) noexcept = default;

    public:
        auto send_receive( IN Architecture::ArchitectureManager< TARGET_ARCH >::SyscallFunction function, IN Lib::Types::uint64_t source_destination ) -> Architecture::ArchitectureManager< TARGET_ARCH >::SyscallStatus;
        /**
         * @brief 发送消息
         */
        auto send( IN Lib::Types::uint64_t destination ) -> Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus;
        /**
         * @brief 接收消息
         */
        auto receive( IN Lib::Types::uint64_t source ) -> Architecture::ArchitectureManager< TARGET_ARCH >::Syscall::SyscallStatus;
    };
}