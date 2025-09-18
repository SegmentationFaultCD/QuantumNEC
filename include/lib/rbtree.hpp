#pragma once
#include <compare>
#include <concepts>
#include <memory>
#include <utility>
namespace Library {
template < typename K, typename T >
    requires std::totally_ordered< K > && std::copyable< T >
class RBTree {
    enum Color : uint64_t {
        RED,
        BLACK
    };

public:
    class RBTreeNode {
    public:
        template < typename _K, typename _T >
            requires std::totally_ordered< _K > && std::copyable< _T >
        friend class RBTree;
        template < typename, typename, typename >
        friend class _RBTree_Iterator;
        template < typename, typename, typename >
        friend class _RBTree_reverse_iterator;
        RBTreeNode( ) = default;
        RBTreeNode( const RBTreeNode & ) = default;
        RBTreeNode( const K &key ) :
            _key { key }, _data { } {}
        RBTreeNode( const K &key, const T &data ) :
            _key { key }, _data { data } {}

        RBTreeNode *_parent = nullptr;
        RBTreeNode *_left = nullptr;
        RBTreeNode *_right = nullptr;
        auto &operator=( T &&data_ ) {
            this->_data = data_;
            return *this;
        }
        auto &data( ) {
            return this->_data;
        }
        auto &key( ) {
            return this->_key;
        }

    private:
        T _data;
        K _key;
        Color _col = RED;
    };
    using Node = RBTreeNode;
    template < typename _T, typename Ptr, typename Ref >
    struct _RBTree_Iterator {
        template < typename, typename >
        friend class _rb_tree;
        using Node = RBTreeNode;
        using self = _RBTree_Iterator;
        _RBTree_Iterator( Node *cur, Node *root ) :
            _cur { cur }, _root { root } {}

    public:
        Ref operator*( ) {
            return _cur->_data;
        }
        Ptr operator->( ) {
            return &( _cur->_data );
        }
        self &operator++( ) {
            Node *cur = _cur;
            if ( _cur == nullptr )
                return *this;
            if ( cur->_right != nullptr ) {
                cur = cur->_right;
                while ( cur->_left != nullptr )
                    cur = cur->_left;
                _cur = cur;
            }
            else {
                Node *parent = cur->_parent;
                while ( parent != nullptr && cur == parent->_right ) {
                    cur = cur->_parent;
                    parent = parent->_parent;
                }
                _cur = parent;
            }
            return *this;
        }
        self &operator--( ) {
            Node *cur = _cur;
            if ( _cur == nullptr )
                _cur = findRightMost( _root );
            else if ( cur->_left != nullptr ) {
                cur = cur->_left;
                while ( cur->_right != nullptr )
                    cur = cur->_right;
                _cur = cur;
            }
            else {
                Node *parent = cur->_parent;
                while ( parent != nullptr && cur == parent->_left ) {
                    cur = cur->_parent;
                    parent = parent->_parent;
                }
                _cur = parent;
            }
            return *this;
        }
        bool operator!=( const self &it ) {
            return _cur != it._cur;
        }
        bool operator==( const self &it ) {
            return _cur == it._cur;
        }
        auto empty( ) {
            return this->_cur == nullptr;
        }

    private:
        Node *findRightMost( Node *root ) {
            if ( root == nullptr )
                return nullptr;
            Node *cur = root;
            while ( cur->_right != nullptr )
                cur = cur->_right;
            return cur;
        }

    private:
        Node *_cur;
        Node *_root;
    };

    template < typename Forward_iterator, typename Ptr, typename Ref >
    class _RBTree_reverse_iterator {
        template < typename, typename >
        friend class _rb_tree;
        _RBTree_reverse_iterator( const Forward_iterator &node ) :
            _cur { node } {}

    public:
        using self = _RBTree_reverse_iterator;
        auto &operator*( ) {
            Forward_iterator tmp = _cur;
            return *( --tmp );
        }
        auto operator->( ) {
            return &( *_cur );
        }
        auto &operator++( ) {
            --_cur;
            return *this;
        }
        auto &operator--( ) {
            ++_cur;
            return *this;
        }
        bool operator!=( const self &rhs ) {
            return _cur != rhs._cur;
        }
        bool operator==( const self &rhs ) {
            return _cur == rhs._cur;
        }

    private:
        Forward_iterator _cur;
    };

public:
    using iterator = _RBTree_Iterator< T, const T *, const T & >;
    using const_iterator = _RBTree_Iterator< T, const T *, const T & >;
    using reverse_iterator = _RBTree_reverse_iterator< iterator, const T *, const T & >;
    using const_reverse_iterator = _RBTree_reverse_iterator< const_iterator, const T *, const T & >;

public:
    bool Is_balance( ) {
        if ( _root == nullptr )
            return true;

        if ( _root->_col != BLACK )
            return false;

        auto cur = _root;
        int numBlack = 0, count = 0;
        while ( cur != nullptr ) {
            if ( cur->_col == BLACK )
                ++numBlack;
            cur = cur->_left;
        }
        return _Is_balance( _root, numBlack, count );
    }
    bool _Is_balance( Node *root, const int k, int countBlack ) {
        if ( root == nullptr ) {
            if ( k == countBlack )
                return true;
            else {
                return false;
            }
        }
        if ( root->_col == BLACK )
            ++countBlack;
        auto parent = root->_parent;
        if ( parent != nullptr && parent->_col == RED && root->_col == RED ) {
            return false;
        }
        return _Is_balance( root->_left, k, countBlack ) && _Is_balance( root->_right, k, countBlack );
    }

    auto begin( ) {
        auto cur = _root;
        if ( cur != nullptr )
            while ( cur->_left != nullptr )
                cur = cur->_left;
        return iterator { cur, _root };
    }
    auto end( ) {
        return iterator { nullptr, _root };
    }
    auto begin( ) const {
        auto cur = _root;
        if ( cur != nullptr )
            while ( cur->_left != nullptr )
                cur = cur->_left;
        return const_iterator { cur, _root };
    }
    auto end( ) const {
        return const_iterator { nullptr, _root };
    }
    auto rbegin( ) {
        return reverse_iterator { this->end( ) };
    }
    auto rend( ) {
        auto cur = _root;
        if ( cur != nullptr )
            while ( cur != nullptr )
                cur = cur->_left;
        return reverse_iterator( this->begin( ) );
    }
    auto rbegin( ) const {
        return const_reverse_iterator { this->end( ) };
    }
    auto rend( ) const {
        auto cur = _root;
        if ( cur != nullptr )
            while ( cur != nullptr )
                cur = cur->_left;
        return const_reverse_iterator { this->begin( ) };
    }

public:
    RBTree( ) = default;

    ~RBTree( ) {
        _destroy( _root );
    }

public:
    auto find( const K &key ) -> T * {
        auto cur = _root;
        while ( cur != nullptr ) {
            auto result = cur->key( ) <=> key;
            if ( result == std::strong_ordering::less )
                cur = cur->_right;
            else if ( result == std::strong_ordering::greater )
                cur = cur->_left;
            else
                return &cur->_data;
        }
        return nullptr;
    }
    // TODO 转换为插入Node而非data

    auto insert( Node &node ) -> std::pair< iterator, bool > {
        if ( _root == nullptr ) {
            this->_root = &node;
            _root->_col = BLACK;
            return { iterator { _root, _root }, true };
        }
        Node *cur = _root;
        Node *parent = nullptr;
        while ( cur != nullptr ) {
            parent = cur;
            if ( cur->key( ) <=> node.key( ) == std::strong_ordering::less )
                cur = cur->_right;
            else if ( cur->key( ) <=> node.key( ) == std::strong_ordering::greater )
                cur = cur->_left;
            else
                return { iterator { cur, _root }, false };
        }
        auto newnode = &node;
        newnode->_parent = parent;
        if ( parent->key( ) <=> node.key( ) == std::strong_ordering::less )
            parent->_right = newnode;
        else
            parent->_left = newnode;
        cur = newnode;
        while ( ( parent != nullptr && parent != _root ) && parent->_col == RED ) {
            auto grandfather = parent->_parent;
            Node *uncle = nullptr;
            if ( parent == grandfather->_right )
                uncle = grandfather->_left;
            else
                uncle = grandfather->_right;
            if ( uncle != nullptr && uncle->_col == RED ) {
                uncle->_col = parent->_col = BLACK;
                grandfather->_col = RED;
                cur = grandfather;
                parent = cur->_parent;
            }
            else {
                if ( parent == grandfather->_left ) {
                    if ( cur == parent->_left )
                        RotateR( grandfather );
                    else
                        RotateLR( grandfather );
                }
                else {
                    if ( cur == parent->_right )
                        RotateL( grandfather );
                    else
                        RotateRL( grandfather );
                }
                break;
            }
        }

        _root->_col = BLACK;
        return { iterator { newnode, _root }, true };
    }
    auto operator[]( std::uint64_t key ) -> T & {
        if ( auto node = this->find( key ); node ) {
            return *node;
        }
        auto new_node = new Node { key };
        this->insert( *new_node );
        return new_node->_data;
    }

    auto erase( const K &key ) {
        auto cur = _root;
        while ( cur != nullptr ) {
            if ( cur->key( ) <=> key == std::strong_ordering::less )
                cur = cur->_right;
            else if ( cur->key( ) <=> key == std::strong_ordering::greater )
                cur = cur->_left;
            else {
                if ( cur->_left == nullptr ) {
                    if ( cur == _root ) {
                        _root = cur->_right;
                        if ( _root != nullptr )
                            _root->_col = BLACK;
                        break;
                    }
                    if ( cur->_right != nullptr ) {
                        auto parent = cur->_parent;
                        cur->_right->_parent = parent;
                        if ( cur == parent->_right )
                            parent->_right = cur->_right;
                        else
                            parent->_left = cur->_right;

                        cur->_right->_col = BLACK;
                    }
                    else {
                        if ( cur->_col == RED ) {
                            auto parent = cur->_parent;
                            if ( cur == parent->_right )
                                parent->_right = nullptr;
                            else
                                parent->_left = nullptr;
                        }
                        else {
                            DeleteFixUp( cur );
                            auto parent = cur->_parent;
                            if ( cur == parent->_right )
                                parent->_right = nullptr;
                            else
                                parent->_left = nullptr;
                        }
                    }
                    return;
                }
                else if ( cur->_right == nullptr ) {
                    if ( cur == _root ) {
                        _root = cur->_left;
                        if ( _root != nullptr )
                            _root->_col = BLACK;
                        break;
                    }
                    if ( cur->_left != nullptr ) {
                        auto parent = cur->_parent;
                        cur->_left->_parent = parent;
                        if ( cur == parent->_right )
                            parent->_right = cur->_left;
                        else
                            parent->_left = cur->_left;
                        cur->_left->_col = BLACK;
                    }
                    else {
                        if ( cur->_col == RED ) {
                            auto parent = cur->_parent;
                            if ( cur == parent->_right )
                                parent->_right = nullptr;
                            else
                                parent->_left = nullptr;
                        }
                        else {
                            DeleteFixUp( cur );
                            auto parent = cur->_parent;
                            if ( cur == parent->_right )
                                parent->_right = nullptr;
                            else
                                parent->_left = nullptr;
                        }
                    }
                    return;
                }
                else {
                    auto rightMin = cur->_left;
                    while ( rightMin->_right != nullptr )
                        rightMin = rightMin->_right;

                    cur->data( ) = rightMin->data( );
                    if ( rightMin->_left != nullptr ) {
                        auto parent = rightMin->_parent;
                        rightMin->_left->_parent = parent;
                        if ( rightMin == parent->_right )
                            parent->_right = rightMin->_left;
                        else
                            parent->_left = rightMin->_left;
                        rightMin->_left->_col = BLACK;
                    }
                    else {
                        if ( rightMin->_col == RED ) {
                            auto parent = rightMin->_parent;
                            if ( rightMin == parent->_right )
                                parent->_right = nullptr;
                            else
                                parent->_left = nullptr;
                        }
                        else {
                            DeleteFixUp( rightMin );
                            auto parent = rightMin->_parent;
                            if ( rightMin == parent->_right )
                                parent->_right = nullptr;
                            else
                                parent->_left = nullptr;
                        }
                    }
                    return;
                }
            }
        }
        return;
    }

private:
    auto DeleteFixUp( Node *node ) {
        auto dbl_black = node;
        while ( dbl_black != _root && ( dbl_black != nullptr && dbl_black->_col != RED ) ) {
            auto parent = dbl_black->_parent;
            Node *sibling = nullptr;
            if ( dbl_black == parent->_left )
                sibling = parent->_right;
            else
                sibling = parent->_left;

            if ( sibling->_col == RED ) {
                Color parentColor = parent->_col;
                Color siblingColor = sibling->_col;
                if ( sibling == parent->_left )
                    RotateR( parent );
                else
                    RotateL( parent );

                parent->_col = siblingColor;
                sibling->_col = parentColor;
            }
            else {
                if ( ( sibling->_left == nullptr || sibling->_left->_col == BLACK ) && ( sibling->_right == nullptr || sibling->_right->_col == BLACK ) ) {
                    sibling->_col = RED;
                    dbl_black = parent;
                }
                else if ( sibling->_left != nullptr && sibling->_left->_col == RED ) {
                    if ( sibling == parent->_left ) {
                        auto sibLeft = sibling->_left;

                        Color siblingColor = sibling->_col;
                        Color parentColor = parent->_col;
                        RotateR( parent );
                        sibLeft->_col = siblingColor;
                        sibling->_col = parentColor;
                        parent->_col = BLACK;
                    }
                    else {
                        auto sibLeft = sibling->_left;

                        Color siblingColor = sibling->_col;
                        Color parentColor = parent->_col;
                        RotateRL( parent );
                        sibLeft->_col = parentColor;
                        sibling->_col = siblingColor;
                        parent->_col = BLACK;
                    }
                    break;
                }
                else if ( sibling->_right != nullptr && sibling->_right->_col == RED ) {
                    if ( sibling == parent->_right ) {
                        auto sibRight = sibling->_right;

                        Color siblingColor = sibling->_col;
                        Color parentColor = parent->_col;
                        RotateL( parent );
                        sibRight->_col = siblingColor;
                        sibling->_col = parentColor;
                        parent->_col = BLACK;
                    }
                    else {
                        auto sibRight = sibling->_right;

                        Color siblingColor = sibling->_col;
                        Color parentColor = parent->_col;
                        RotateLR( parent );
                        sibRight->_col = parentColor;
                        sibling->_col = siblingColor;
                        parent->_col = BLACK;
                    }
                    break;
                }
                else {
                    break;
                }
            }
        }
        if ( dbl_black != nullptr )
            dbl_black->_col = BLACK;
        return;
    }

    auto RotateR( Node *parent ) {
        auto SubL = parent->_left;
        auto ppnode = parent->_parent;

        parent->_parent = SubL;
        parent->_left = SubL->_right;
        if ( SubL->_right != nullptr )
            SubL->_right->_parent = parent;

        SubL->_right = parent;
        if ( parent == _root )
            _root = SubL;
        else {
            if ( ppnode->_left == parent )
                ppnode->_left = SubL;
            else
                ppnode->_right = SubL;
        }

        SubL->_parent = ppnode;
        parent->_col = RED;
        SubL->_col = BLACK;
    }
    auto RotateL( Node *parent ) {
        auto SubR = parent->_right;
        auto ppnode = parent->_parent;

        parent->_parent = SubR;
        parent->_right = SubR->_left;
        if ( SubR->_left != nullptr )
            SubR->_left->_parent = parent;

        SubR->_left = parent;
        if ( parent == _root )
            _root = SubR;
        else {
            if ( ppnode->_left == parent )
                ppnode->_left = SubR;
            else
                ppnode->_right = SubR;
        }

        SubR->_parent = ppnode;
        parent->_col = RED;
        SubR->_col = BLACK;
    }
    auto RotateLR( Node *parent ) {
        auto SubL = parent->_left;

        RotateL( SubL );
        RotateR( parent );
    }
    auto RotateRL( Node *parent ) {
        auto SubR = parent->_right;

        RotateR( SubR );
        RotateL( parent );
    }

    // 递归子函数
private:
    auto _destroy( Node *cur ) {
        if ( cur == nullptr )
            return;
        _destroy( cur->_left );
        _destroy( cur->_right );
        cur = nullptr;
    }

private:
    Node *_root = nullptr;
};
}     // namespace Library