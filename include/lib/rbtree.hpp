#pragma once
#include <lib/Uefi.hpp>
#include <utility>
PUBLIC namespace QuantumNEC {
    template < typename T, typename Keyofvalue >
    class RBTree {
    public:
        // 红黑树节点的定义
        class RBTreeNode {
            friend RBTree;

        public:
            enum class Color {
                RED,
                BLACK
            };

        public:
            RBTreeNode( Keyofvalue key, T *data = NULL, Color color = Color::RED ) noexcept
                // 这里构造节点的时候颜色默认给为红色因为如果给为黑色就有可能会破坏当前红黑树的性质，导致每条路径的黑色节点个数不同
                :
                _left( NULL ), _right( NULL ), _parent( NULL ), _color( color ), _key( key ), _data( data ) {
            }

        private:
            RBTreeNode *_left;       // 节点的左孩子
            RBTreeNode *_right;      // 节点的右孩子
            RBTreeNode *_parent;     // 节点的双亲(红黑树需要旋转，为了实现简单给出该字段)
            Color       _color;      // 节点的颜色
        public:
            Keyofvalue _key;      // 节点的键值
            T         *_data;     // 节点的值域
        };
        using Node = RBTreeNode;

        // 红黑树迭代器：
        template < typename _T, typename Ref, typename Ptr >
        struct RBTreeIterator {
            using self = RBTreeIterator< T, Ref, Ptr >;
            // 构造函数就将红黑树的节点指针传入进来：
            RBTreeIterator( Node *node = nullptr ) :
                _pnode { node } {
            }
            // 迭代器解引用：
            Ref operator*( ) {
                return *_pnode->_data;
            }
            Ptr operator->( ) {
                return ( &operator*( ) );
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
                self temp = *this;
                this->decreament( );
                return temp;
            }
            bool operator==( const self &s ) const {
                return _pnode == s._pnode;
            }
            bool operator!=( const self &s ) const {
                return _pnode != s._pnode;
            }
            // 将当前迭代器指针的值放到后面大的值上
            void increament( ) {
                // 如果当前迭代器存在右子树的时候我们将_pnode更新到右子树
                if ( _pnode->_right ) {
                    _pnode = _pnode->_right;
                    // 去右子树中找最小的节点：
                    while ( _pnode->_left ) {
                        _pnode = _pnode->_left;
                    }
                }
                else {
                    auto parent = _pnode->_parent;
                    while ( parent->_right == _pnode ) {
                        _pnode = parent;
                        parent = _pnode->_parent;
                    }
                    if ( _pnode->_right != parent ) {
                        _pnode = parent;
                    }
                }
            }
            void decreament( ) {
                if ( _pnode->_parent->_parent == _pnode && _pnode->_color == Node::Color::RED ) {
                    _pnode = _pnode->_right;
                }
                // 如果当前的pnode的左子树存在那么我们就将节点放在左子树
                else if ( _pnode->_left ) {
                    _pnode = _pnode->_left;
                    while ( _pnode->_right ) {
                        _pnode = _pnode->_right;
                    }
                }
                else {
                    auto parent = _pnode->_parent;
                    // 这里如果_pnode到了begin的位置就不可以再减了
                    while ( _pnode == parent->_left ) {
                        _pnode = parent;
                        parent = _pnode->_parent;
                    }
                    _pnode = parent;
                }
            }

        private:
            Node *_pnode;
        };

    private:
        Node parent;     // 始祖

    public:
        using iterator       = RBTreeIterator< T, T &, T       *>;
        using const_iterator = const RBTreeIterator< T, T &, T * >;

        iterator end( ) {
            return iterator( NULL );
        }
        iterator begin( ) {
            return iterator( this->left_most( ) );
        }
        const_iterator end( ) const {
            return const_iterator( NULL );
        }
        const_iterator begin( ) const {
            return const_iterator( this->left_most( ) );
        }

    public:
        auto init( VOID ) {
            this->parent._color = RBTreeNode::Color::RED;
        }
        explicit RBTree( VOID ) noexcept :
            parent { 0 } {
            this->init( );
        }

        auto insert( IN Node &_node )     // 插入节点
        {
            auto z = &_node;

            if ( !this->_root ) {     // 为空树
                this->_root          = z;
                this->_root->_parent = &this->parent;
                this->_root->_color  = Node::Color::BLACK;
                this->_root->_left   = NULL;
                this->_root->_right  = NULL;
                this->_size          = 1;
                this->_nil           = this->_root;

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
        ~RBTree( ) {
        }

        // 查找方法
        auto search( IN Keyofvalue key ) -> Node * {
            auto *node = this->_root->_parent;
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
        auto swap( IN RBTree< T, Keyofvalue > _t ) {
            std::swap( _root, _t._root );
        }

        auto size( ) const {
            return this->_size;
        }
        auto empty( ) const {
            return !this->_size;
        }

        auto inoder( ) {
            auto *_root = this->_root->_parent;
            this->mid( _root );
        }

    private:
        // // 中序遍历：
        auto mid( Node *root ) {
            if ( root ) {
                this->mid( root->_left );
                this->mid( root->_right );
            }
        }
        auto left_most( ) {
            auto left = _root->_left;
            while ( left && left->_left )
                left = left->_left;
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
        uint64_t _size;
        // Node    *_head;
        Node *_root;
        Node *_nil;
    };
}