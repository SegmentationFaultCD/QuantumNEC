#pragma once
#include <lib/Uefi.hpp>
#include <utility>
PUBLIC namespace QuantumNEC {
    template < typename T, typename Keyofvalue >
    class RedBlackTree {
    public:
        // 红黑树节点的定义
        class RedBlackTreeNode {
            friend RedBlackTree;

        public:
            enum class Color {
                RED,
                BLACK
            };

        public:
            RedBlackTreeNode( Keyofvalue key, T *data = NULL, Color color = Color::RED ) noexcept
                // 这里构造节点的时候颜色默认给为红色因为如果给为黑色就有可能会破坏当前红黑树的性质，导致每条路径的黑色节点个数不同
                :
                _left( NULL ), _right( NULL ), _parent( NULL ), _color( color ), _key( key ), _data( data ) {
            }

        public:
            RedBlackTreeNode *_left;       // 节点的左孩子
            RedBlackTreeNode *_right;      // 节点的右孩子
            RedBlackTreeNode *_parent;     // 节点的双亲(红黑树需要旋转，为了实现简单给出该字段)
            Color             _color;      // 节点的颜色
        public:
            T         *_data;     // 节点的值域
            Keyofvalue _key;      // 节点的键值
        };
        using Node = RedBlackTreeNode;

        // 红黑树迭代器：
        template < typename _T, typename Ref, typename Ptr >
        struct RedBlackTreeIterator {
            using self = RedBlackTreeIterator< T, Ref, Ptr >;
            // 构造函数就将红黑树的节点指针传入进来：
            RedBlackTreeIterator( Node *node = NULL ) :
                _pnode { node } {
            }
            // 迭代器解引用：
            Ref operator*( ) {
                return *_pnode->_data;
            }
            Ptr operator->( ) {
                return _pnode->_data;
            }
            // 迭代器加加:前置加加
            self operator++( ) {
                this->increament( );
                return *this;
            }
            self operator++( int ) {
                self temp = *this;
                this->increament( );
                return temp;
            }
            self operator--( ) {
                this->decreament( );
                return *this;
            }
            self operator--( int ) {
                auto temp = *this;
                this->decreament( );
                return temp;
            }
            auto operator==( const self &s ) const {
                return _pnode == s._pnode;
            }
            auto operator!=( const self &s ) const {
                return _pnode != s._pnode;
            }
            // 将当前迭代器指针的值放到后面大的值上
            auto increament( ) {
                if ( _pnode->_right ) {
                    _pnode = _pnode->_right;
                    while ( _pnode->_left ) {
                        _pnode = _pnode->_left;
                    }
                }
                else     // 2.若结点的右子树不存在，则找到结点不是其父亲右孩子的结点
                {
                    auto parent = _pnode->_parent;
                    while ( parent && parent->_right == _pnode ) {
                        _pnode = parent;
                        parent = _pnode->_parent;
                    }
                    _pnode = parent;
                }
            }
            auto decreament( ) {
                if ( _pnode->_left ) {
                    _pnode = _pnode->_left;
                    while ( _pnode->_right )
                        _pnode = _pnode->_right;
                }
                else {
                    // 2.若结点的左孩子不存在，则向上寻找直到其不为父亲的左孩子
                    auto parent = _pnode->_parent;
                    while ( parent && parent->_left == _pnode ) {
                        _pnode = parent;
                        parent = _pnode->_parent;
                    }
                    _pnode = parent;
                }
            }

        private:
            Node *_pnode;
        };

    public:
        using Iterator      = RedBlackTreeIterator< T, T &, T      *>;
        using ConstIterator = const RedBlackTreeIterator< T, T &, T * >;

        Iterator end( ) {
            return Iterator { };
        }
        Iterator begin( ) {
            return Iterator { this->left_most( ) };
        }
        ConstIterator end( ) const {
            return ConstIterator { };
        }
        ConstIterator begin( ) const {
            return ConstIterator { this->left_most( ) };
        }

    public:
        auto init( ) {
            this->_nil  = NULL;
            this->_root = NULL;
            this->_size = 0;
        }
        explicit RedBlackTree( void ) noexcept {
            this->init( );
        }

        auto insert( Node &_node )     // 插入节点
        {
            auto z = &_node;
            if ( !this->_root ) {     // 为空树
                this->_root         = z;
                this->_root->_color = Node::Color::BLACK;
                this->_root->_left  = NULL;
                this->_root->_right = NULL;
                this->_size         = 1;
                this->_nil          = this->_root;
                return;
            }
            auto *y = this->_nil;
            auto *x = this->_root;
            while ( x != this->_nil ) {
                y = x;
                if ( z->_key < x->_key )
                    x = x->_left;
                else if ( z->_key > x->_key )
                    x = x->_right;
                else
                    return;
            }

            z->_parent = y;
            if ( y == this->_nil )
                this->_root = z;
            else {
                if ( y->_key > z->_key )
                    y->_left = z;
                else
                    y->_right = z;
            }

            z->_left = z->_right = this->_nil;
            z->_color            = Node::Color::RED;
            _size++;
            this->insert_fixup( z );
        }

        auto remove( Node &node ) {
            auto y = this->_nil;
            auto x = this->_nil;
            auto z = &node;
            if ( ( z->_left == this->_nil ) || ( z->_right == this->_nil ) ) {
                y = z;
            }
            else {
                y = this->successor( z );
            }

            if ( y->_left != this->_nil )
                x = y->_left;
            else if ( y->_right != this->_nil )
                x = y->_right;
            x->_parent = y->_parent;
            if ( y->_parent == this->_nil )
                this->_root = x;
            else if ( y == y->_parent->_left )
                y->_parent->_left = x;
            else
                y->_parent->_right = x;

            if ( y != z ) {
                z->_key  = y->_key;
                z->_data = y->_data;
            }
            // 调整
            if ( y->_color == Node::Color::BLACK ) {
                this->remove_fixup( x );
            }
            this->_size--;
        }

        auto clear( ) {
            this->remove( *this->_root );
            this->_size = 0;
        }
        ~RedBlackTree( ) {
        }

        // 查找方法
        auto search( Keyofvalue key ) -> Node * {
            auto *node = this->_root;

            while ( node != this->_nil ) {
                if ( key < node->_key ) {
                    node = node->_left;
                }
                else if ( key > node->_key ) {
                    node = node->_right;
                }
                else {
                    return node;
                }
            }

            return this->_nil;
        }
        auto swap( RedBlackTree< T, Keyofvalue > &_t ) {
            std::swap( _root, _t._root );
        }

        auto size( ) const {
            return this->_size;
        }
        auto empty( ) const {
            return !this->_size;
        }

        // // 中序遍历：
        auto translate( auto func ) -> void
            requires std::invocable< decltype( func ), T & >
        {
            auto _helper_ = [ &func ]( this auto &self, Node *root ) -> BOOL {
                if ( root ) {
                    if ( func( *root->_data ) ) {
                        return FALSE;
                    }

                    if ( self( root->_left ) ) {
                        return FALSE;
                    }
                    if ( self( root->_right ) ) {
                        return FALSE;
                    }
                }
                return TRUE;
            };

            _helper_( this->_root );
            return;
        }

    private:
        auto left_most( ) {
            auto left = _root->_left;
            while ( left && left->_left ) {
                left = left->_left;
            }
            return left;
        }
        auto right_most( ) {
            auto right = _root->_right;
            while ( right && right->_right )
                right = right->_right;
            return right;
        }
        auto insert_fixup( Node *node ) {
            while ( node->_parent->_color == Node::Color::RED ) {
                auto grandparent = node->_parent->_parent;
                if ( node->_parent == grandparent->_left ) {
                    auto uncle = grandparent->_right;

                    if ( uncle->_color == Node::Color::RED )     // 叔父结点为红色
                    {
                        node->_parent->_color = Node::Color::BLACK;
                        uncle->_color         = Node::Color::BLACK;
                        grandparent->_color   = Node::Color::RED;
                        node                  = grandparent;
                    }
                    else {
                        if ( node == node->_parent->_right ) {
                            node = node->_parent;
                            this->rotate_left( node );
                        }
                        node->_parent->_color = Node::Color::BLACK;
                        grandparent->_color   = Node::Color::RED;
                        // 祖父结点旋转
                        this->rotate_right( grandparent );
                    }
                }
                else {
                    auto *uncle = grandparent->_left;
                    if ( uncle->_color == Node::Color::RED )     // 叔父结点为红色
                    {
                        node->_parent->_color = Node::Color::BLACK;
                        uncle->_color         = Node::Color::BLACK;
                        grandparent->_color   = Node::Color::RED;
                        uncle                 = uncle->_parent->_parent;
                    }
                    else {
                        if ( node == node->_parent->_left ) {
                            node = node->_parent;
                            this->rotate_right( node );
                        }
                        node->_parent->_color = Node::Color::BLACK;
                        grandparent->_color   = Node::Color::RED;
                        this->rotate_left( grandparent );
                    }
                }
            }
            this->_root->_color = Node::Color::BLACK;
        }
        // 左单旋
        auto rotate_left( Node *x ) {
            auto y    = x->_right;
            x->_right = y->_left;
            if ( y->_left != this->_nil ) {
                y->_left->_parent = x;
            }
            y->_parent = x->_parent;
            if ( x->_parent == this->_nil ) {
                this->_root = y;
            }
            else if ( x == x->_parent->_left ) {
                x->_parent->_left = y;
            }
            else {
                x->_parent->_right = y;
            }
            y->_left   = x;
            x->_parent = y;
        }
        // 右单旋
        auto rotate_right( Node *y ) {
            auto x   = y->_left;
            y->_left = x->_right;
            if ( x->_right != this->_nil ) {
                x->_right->_parent = y;
            }
            x->_parent = y->_parent;
            if ( y->_parent == this->_nil ) {
                this->_root = x;
            }
            else if ( y == y->_parent->_right ) {
                y->_parent->_right = x;
            }
            else {
                y->_parent->_left = x;
            }
            // 3
            x->_right  = y;
            y->_parent = x;
        }

        auto mini( Node *x ) {
            while ( x->_left != this->_nil ) {
                x = x->_left;
            }
            return x;
        }

        auto maxi( Node *x ) {
            while ( x->_right != this->_nil ) {
                x = x->_right;
            }
            return x;
        }

        auto successor( Node *x ) {
            auto y = x->_parent;
            if ( x->_right != this->_nil ) {
                return this->mini( x->_right );
            }
            while ( ( y != this->_nil ) && ( x == y->_right ) ) {
                x = y;
                y = y->_parent;
            }
            return y;
        }
        // 调整
        void remove_fixup( Node *x ) {
            while ( ( x != this->_root ) && ( x->_color == Node::Color::BLACK ) ) {
                if ( x == x->_parent->_left ) {
                    auto w = x->_parent->_right;
                    if ( w->_color == Node::Color::RED ) {
                        w->_color          = Node::Color::BLACK;
                        x->_parent->_color = Node::Color::RED;
                        this->rotate_left( x->_parent );
                        w = x->_parent->_right;
                    }
                    if ( ( w->_left->_color == Node::Color::BLACK ) && ( w->_right->_color == Node::Color::BLACK ) ) {
                        w->_color = Node::Color::RED;
                        x         = x->_parent;
                    }
                    else {
                        if ( w->_right->_color == Node::Color::BLACK ) {
                            w->_left->_color = Node::Color::BLACK;
                            w->_color        = Node::Color::RED;
                            rotate_right( w );
                            w = x->_parent->_right;
                        }

                        w->_color          = x->_parent->_color;
                        x->_parent->_color = Node::Color::BLACK;
                        w->_right->_color  = Node::Color::BLACK;
                        rotate_left( x->_parent );

                        x = this->_root;
                    }
                }
                else {
                    auto w = x->_parent->_left;
                    if ( w->_color == Node::Color::RED ) {
                        w->_color          = Node::Color::BLACK;
                        x->_parent->_color = Node::Color::RED;
                        rotate_right( x->_parent );
                        w = x->_parent->_left;
                    }

                    if ( ( w->_left->_color == Node::Color::BLACK ) && ( w->_right->_color == Node::Color::BLACK ) ) {
                        w->_color = Node::Color::RED;
                        x         = x->_parent;
                    }
                    else {
                        if ( w->_left->_color == Node::Color::BLACK ) {
                            w->_right->_color = Node::Color::BLACK;
                            w->_color         = Node::Color::RED;
                            rotate_left( w );
                            w = x->_parent->_left;
                        }

                        w->_color          = x->_parent->_color;
                        x->_parent->_color = Node::Color::BLACK;
                        w->_left->_color   = Node::Color::BLACK;
                        rotate_right( x->_parent );

                        x = this->_root;
                    }
                }
            }

            x->_color = Node::Color::BLACK;
        }

    private:
        uint64_t _size = 0;
        Node    *_root = nullptr;
        Node    *_nil  = nullptr;
    };
}