#pragma once
#include <lib/Uefi.hpp>
#include <concepts>

PUBLIC namespace QuantumNEC::Lib {
    PUBLIC struct ListNode
    {
        ListNode *prev { }; /* 上一个节点 */
        ListNode *next { }; /* 下一个节点 */
        VOID *container { };
        explicit ListNode( VOID ) noexcept = default;

        /**
         * @brief 判断上一节点十分为空
         * @param node 要判断的节点
         */
        auto is_empty_prev( IN ListNode *node ) -> ListNode * {
            return node->prev ? node->prev : NULL;
        }
        /**
         * @brief 判断下一节点十分为空
         * @param node 要判断的节点
         */
        auto is_empty_next( IN ListNode *node ) -> ListNode * {
            return node->next ? node->next : NULL;
        }
        auto operator=( ListNode &node ) -> ListNode & {
            this->container = node.container;
            this->next = node.next;
            this->prev = node.prev;
            return *this;
        }
    };
    // 双向链表
    PUBLIC class ListTable
    {
        ListNode _head { }; /* 链表头 */
        ListNode _end { };  /* 链表尾 */
    public:
        auto init( VOID ) {
            this->_head.prev = NULL;
            this->_head.next = &this->_end;
            this->_end.prev = &this->_head;
            this->_end.next = NULL;
        }
        ListTable( VOID ) noexcept {
            this->init( );
        }

        ~ListTable( VOID ) noexcept = default;
        auto operator=( ListTable &lt ) -> ListTable & {
            this->_head = lt._head;
            this->_end = lt._end;
            return *this;
        }

    public:
        auto begin( ) {
            return this->_head.next;
        }
        /**
         * @brief 插入节点到链表末尾
         * @param New 要添加的元素的指针
         */
        auto append( IN OUT ListNode &New ) {
            this->insert( &New, &this->_end );
        }

        /**
         * @brief 插入节点到链表开头
         * @param New  要添加的元素的指针
         */
        auto push( IN OUT ListNode &New ) {
            this->insert( &New, this->_head.next );
        }
        /**
         * @brief 删除节点
         * @param entry 要弹出的节点
         */
        auto remove( IN OUT ListNode &entry ) {
            entry.next->prev = entry.prev;
            entry.prev->next = entry.next;
        }
        /**
         * @brief 将链表第一个元素弹出并返回
         * @return 第一个元素
         */
        auto pop( VOID ) {
            auto node { this->_head.next };
            this->remove( *node );
            return node;
        }
        /**
         * @brief 判断链表是否为空
         */
        auto is_empty( VOID ) {
            return this->_head.next == &this->_end ? TRUE : FALSE;
        }
        /**
         * @brief 在链表中查找节点
         * @param objnode 要查找的节点
         * @retval false 查找失败
         * @retval true  找到元素
         */
        auto find( IN ListNode &objnode ) {
            auto node { this->_head.next };
            while ( node != &( this->_end ) ) {
                if ( node == &objnode )
                    return TRUE;
                node = node->next;
            }
            return FALSE;
        }
        /**
         * @brief 遍历列表内所有元素，逐个判断是否有符合条件的元素
         * @param func 回调函数
         * @param arg 给func用来判断下一节点是否符合条件
         * @return 找到符合条件的元素返回元素指针，否则返回 NULL
         */
        auto traversal( IN CONST auto func, IN CONST auto arg ) -> ListNode *
            requires std::invocable< decltype( func ), ListNode *, decltype( arg ) >
        {
            auto node { this->_head.next };
            /* 如果队列为空，就必然没有符合条件的结点，故直接返回 NULL */
            if ( this->is_empty( ) )
                return NULL;
            while ( node != &this->_end ) {
                if ( func(
                         node,
                         arg ) )     // func 返回
                                     // ture，则认为该元素在回调函数中符合条件，命中，故停止继续遍历
                    return node;
                // 若回调函数 func 返回 true，则继续遍历
                node = node->next;
            }
            return NULL;
        }
        auto length( VOID ) {
            uint64_t length { };
            for ( auto node { this->_head.next }; node != &this->_end; ++length ) {
                node = node->next;
            }
            return length;
        }
        auto insert( IN OUT ListNode *node, IN OUT ListNode *in_before ) -> VOID {
            in_before->prev->next = node;
            node->prev = in_before->prev;
            node->next = in_before;
            in_before->prev = node;
        };
        auto back( VOID ) {
            return this->_end.prev;
        }
        auto end( VOID ) {
            return this->_end.prev;
        }
    };
}
