#pragma once
#include <concepts>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Lib {
// 双向链表
template < typename T >
class ListTable {
public:
    struct ListNode {
        friend ListTable;

    public:
        ListNode *prev { }; /* 上一个节点 */
        ListNode *next { }; /* 下一个节点 */
    private:
        T *container { };

    public:
        explicit ListNode( void ) noexcept = default;

        /**
         * @brief 判断上一节点十分为空
         * @param node 要判断的节点
         */
        auto *is_empty_prev( IN ListNode *node ) {
            return node->prev ? node->prev : NULL;
        }
        /**
         * @brief 判断下一节点十分为空
         * @param node 要判断的节点
         */
        auto *is_empty_next( IN ListNode *node ) {
            return node->next ? node->next : NULL;
        }
        auto &operator=( ListNode &node ) {
            this->container = node.container;
            this->next      = node.next;
            this->prev      = node.prev;
            return *this;
        }
        auto &operator=( T &data ) {
            this->container = &data;
            return *this;
        }

        auto &operator*( ) {
            return *this->container;
        }
        auto *operator->( ) {
            return this->container;
        }
    };

public:
    using Node = ListNode;

private:
    ListNode _head { }; /* 链表头 */
    ListNode _end { };  /* 链表尾 */
public:
    auto init( void ) {
        this->_head.prev = NULL;
        this->_head.next = &this->_end;
        this->_end.prev  = &this->_head;
        this->_end.next  = NULL;
    }
    ListTable( void ) noexcept {
        this->init( );
    }

    ~ListTable( void ) noexcept = default;
    auto &operator=( ListTable &lt ) {
        this->_head = lt._head;
        this->_end  = lt._end;
        return *this;
    }

public:
    template < class _T, class Ref, class Ptr >
    struct ListIterator {
        using self = ListIterator< _T, Ref, Ptr >;
        // 构造函数就将红黑树的节点指针传入进来：
        ListIterator( Node *node = nullptr ) :
            _pnode { node } {
        }
        // 迭代器解引用：
        Ref operator*( ) {
            return *_pnode->container;
        }
        Ptr operator->( ) {
            return _pnode->container;
        }
        // 迭代器加加:前置加加
        self operator++( ) {
            this->_pnode = this->_pnode->next;
            return *this;
        }
        self operator++( int ) {
            self temp    = *this;
            this->_pnode = this->_pnode->next;
            return temp;
        }
        self operator--( ) {
            this->_pnode = this->_pnode->prev;
            return *this;
        }
        self operator--( int ) {
            self temp    = *this;
            this->_pnode = this->_pnode->prev;
            return temp;
        }

        bool operator==( const self &s ) const {
            return _pnode == s._pnode;
        }
        bool operator!=( const self &s ) const {
            return _pnode != s._pnode;
        }
        Node *_pnode;
    };
    using iterator       = ListIterator< T, T &, T * >;
    using const_iterator = const ListIterator< T, T &, T * >;
    auto end( ) {
        return iterator { &_end };
    }
    auto begin( ) {
        return iterator { _head.next };
    }
    auto end( ) const {
        return const_iterator { &_end };
    }
    auto begin( ) const {
        return const_iterator { _head.next };
    }

public:
    /**
     * @brief 插入节点到链表末尾
     * @param New 要添加的元素的指针
     */
    auto append( IN OUT Node &New ) {
        this->insert( &New, &this->_end );
    }

    /**
     * @brief 插入节点到链表开头
     * @param New  要添加的元素的指针
     */
    auto push( IN OUT Node &New ) {
        this->insert( &New, this->_head.next );
    }
    /**
     * @brief 删除节点
     * @param entry 要弹出的节点
     */
    auto remove( IN OUT Node &entry ) {
        if ( entry.next && entry.prev ) {
            entry.next->prev = entry.prev;
            entry.prev->next = entry.next;
        }
    }
    /**
     * @brief 将链表第一个元素弹出并返回
     * @return 第一个元素
     */
    auto pop( void ) {
        auto node { this->_head.next };
        this->remove( *node );
        return node;
    }
    /**
     * @brief 判断链表是否为空
     */
    auto is_empty( void ) {
        return this->_head.next == &this->_end ? true : false;
    }
    /**
     * @brief 在链表中查找节点
     * @param objnode 要查找的节点
     * @retval false 查找失败
     * @retval true  找到元素
     */
    auto find( IN Node &objnode ) {
        auto node { this->_head.next };
        while ( node != &( this->_end ) ) {
            if ( node == &objnode )
                return true;
            node = node->next;
        }
        return false;
    }

    auto length( void ) {
        uint64_t length { };
        for ( auto node { this->_head.next }; node != &this->_end; ++length ) {
            node = node->next;
        }
        return length;
    }
    auto insert( IN OUT Node *node, IN OUT Node *in_before ) {
        in_before->prev->next = node;
        node->prev            = in_before->prev;
        node->next            = in_before;
        in_before->prev       = node;
    };
    auto back( void ) {
        return this->_end.prev;
    }
};
}     // namespace QuantumNEC::Lib
