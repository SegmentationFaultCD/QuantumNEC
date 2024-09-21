#pragma once
#include <Lib/Uefi.hpp>
#include <Lib/list.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC constexpr CONST auto NO_TASK { 0x1919810 };
    PUBLIC constexpr CONST auto ANY { 0x018919 };
    PUBLIC constexpr CONST auto INTERRUPT { 0x114514 };
    PUBLIC class Message
    {
    private:
        union {
            struct MSG1
            {
                uint32_t i1;
                uint32_t i2;
                uint32_t i3;
                uint32_t i4;
                explicit MSG1( VOID ) noexcept = default;
            } volatile msg1;
            struct MSG2
            {
                VOID *p1;
                VOID *p2;
                VOID *p3;
                VOID *p4;
                explicit MSG2( VOID ) noexcept = default;
            } volatile msg2;
            struct MSG3
            {
                uint32_t i1;
                uint32_t i2;
                uint32_t i3;
                uint32_t i4;
                uint64_t l1;
                uint64_t l2;
                VOID *p1;
                VOID *p2;
                explicit MSG3( VOID ) noexcept = default;

            } volatile msg3;
        } message;
        uint64_t source;                /* 发出这个消息的进程pid */
        uint32_t type;                  /* 消息类型 */
        uint64_t send_to;               /* 记录进程想要向谁发送消息 */
        uint64_t receive_from;          /* 记录进程想要从谁获取消息 */
        uint64_t has_int_message;       /* 有来自中断的消息,这个结构被中断处理程序置为1 */
        Lib::ListTable sender_list { }; /* 如果有进程向这个进程发送消息, 但本进程没有要接收消息,那么发送信息的进程将自己的通任务队列加入这个队列 */

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