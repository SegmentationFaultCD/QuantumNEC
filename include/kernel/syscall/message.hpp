#pragma once
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/syscall/syscall.hpp>
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto NO_TASK { 0x1919810 };
    PUBLIC constexpr CONST auto ANY { 0x018919 };
    PUBLIC constexpr CONST auto INTERRUPT { 0x114514 };
    PUBLIC class Message {
    private:
        union {
            struct MSG1 {
                uint32_t i1;
                uint32_t i2;
                uint32_t i3;
                uint32_t i4;
                explicit MSG1( VOID ) noexcept = default;
                auto operator=( MSG1 &msg ) -> MSG1 & {
                    this->i1 = msg.i1;
                    this->i2 = msg.i2;
                    this->i3 = msg.i3;
                    this->i4 = msg.i4;
                    return *this;
                }
            } msg1;
            struct MSG2 {
                VOID *p1;
                VOID *p2;
                VOID *p3;
                VOID *p4;
                auto  operator=( MSG2 &msg ) -> MSG2  &{
                    this->p1 = msg.p1;
                    this->p2 = msg.p2;
                    this->p3 = msg.p3;
                    this->p4 = msg.p4;
                    return *this;
                }
                explicit MSG2( VOID ) noexcept = default;
            } msg2;
            struct MSG3 {
                uint32_t i1;
                uint32_t i2;
                uint32_t i3;
                uint32_t i4;
                uint64_t l1;
                uint64_t l2;
                VOID    *p1;
                VOID    *p2;
                auto     operator=( MSG3 &msg ) -> MSG3     &{
                    this->p1 = msg.p1;
                    this->p2 = msg.p2;
                    this->l1 = msg.l1;
                    this->l2 = msg.l2;
                    this->i1 = msg.i1;
                    this->i2 = msg.i2;
                    this->i3 = msg.i3;
                    this->i4 = msg.i4;
                    return *this;
                }
                explicit MSG3( VOID ) noexcept = default;

            } msg3;
        } message;
        uint64_t source;          /* 发出这个消息的进程pid */
        uint32_t type;            /* 消息类型 */
        uint64_t send_to;         /* 记录进程想要向谁发送消息 */
        uint64_t receive_from;    /* 记录进程想要从谁获取消息 */
        uint64_t has_int_message; /* 有来自中断的消息,这个结构被中断处理程序置为1 */
        // Lib::ListTable< PCB > sender_list { };       /* 如果有进程向这个进程发送消息, 但本进程没有要接收消息,那么发送信息的进程将自己的通任务队列加入这个队列 */
        // Lib::ListNode         message_task_node;     // 运行任务队列 连接每个任务
    public:
        explicit Message( VOID ) noexcept = default;
        ~Message( VOID ) noexcept         = default;
        auto operator=( Message &msg ) -> Message & {
            this->message.msg1 = msg.message.msg1;
            this->message.msg2 = msg.message.msg2;
            this->message.msg3 = msg.message.msg3;
            return *this;
        }

    public:
        /**
         * @brief 发送消息
         */
        auto send( IN uint64_t destination ) -> Syscall::Status;
        /**
         * @brief 接收消息
         */
        auto receive( IN uint64_t source ) -> Syscall::Status;
    };
}