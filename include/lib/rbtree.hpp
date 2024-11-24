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
            bool operator==( const self &s ) const {
                return _pnode == s._pnode;
            }
            bool operator!=( const self &s ) const {
                return _pnode != s._pnode;
            }
            Node *_pnode;
        };

    private:
        Node start_node;

    public:
        using iterator       = RBTreeIterator< T, T &, T       *>;
        using const_iterator = const RBTreeIterator< T, T &, T * >;
        iterator end( ) {
            return iterator( _head );
        }
        iterator begin( ) {
            return iterator( _head->_left );
        }
        const_iterator end( ) const {
            return const_iterator( _head );
        }
        const_iterator begin( ) const {
            return const_iterator( _head->_left );
        }

    public:
        auto init( VOID ) {
            start_node._key   = 0;
            start_node._color = RBTreeNode::Color::RED;
            _head             = &start_node;
            _size             = 0;
            _head->_left      = _head;
            _head->_right     = _head;
        }
        explicit RBTree( VOID ) noexcept :
            start_node { 0 } {
            this->init( );
        }

        auto insert( IN Node &_node )     // 插入节点
        {
            auto node = &_node;
            // 先按照二叉搜索树的方式插入

            auto *&_root = this->get_root( );

            if ( !_root ) {     // 为空树
                _root          = node;
                node->_color   = Node::Color::RED;
                _root->_parent = this->_head;
            }
            else {     // 树非空
                auto cur    = _root;
                auto parent = this->_head;
                while ( cur ) {
                    parent = cur;
                    if ( node->_key < ( cur->_key ) ) {
                        cur = cur->_left;
                    }
                    else if ( node->_key > ( cur->_key ) ) {
                        cur = cur->_right;
                    }
                    else {     // 我们这里不允许插入相同值域的节点
                        return FALSE;
                    }
                }
                cur = node;
                if ( node->_key < ( parent->_key ) ) {
                    parent->_left = cur;
                }
                else {
                    parent->_right = cur;
                }
                cur->_parent = parent;
                // 插入成功之后我们调整当前红黑树的节点：
                // 这里我们在插入红黑树中调整的时候只有当第一中情况才继续向上更新节点，那么我们只要考虑第一中情况的终止条件即可
                // 第一中情况中如果parent为红色节点那么当前节点就需要继续向上更新，但是我们将head节点也设为红色那么当我们parent
                // 节点更新到head节点那么当前也就不更新了
                while ( parent != _head && parent->_color == Node::Color::RED ) {
                    // 插入节点双亲为黑色：
                    if ( parent->_color == Node::Color::BLACK ) {
                        break;
                    }
                    else {                                                          // 插入节点双亲为红色
                        auto grandparent = parent->_parent;                         // 这里如果双亲的节点是红色那么双亲一定是有双亲节点的
                        if ( parent == grandparent->_left ) {                       // 第一大类插入节点在红黑树的左子树：
                            auto uncle = grandparent->_right;                       // 当前节点的叔叔节点
                            if ( uncle && uncle->_color == Node::Color::RED ) {     // 第一种情况：叔叔节点存在而且为红色：
                                parent->_color      = Node::Color::BLACK;
                                uncle->_color       = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                cur                 = grandparent;
                                parent              = cur->_parent;
                            }
                            // 第二三种情况：
                            else {
                                // 因为我们要将第三种情况转化为第二种情况处理所以我们先写第三种情况：cur插在内侧
                                if ( cur == parent->_right ) {
                                    this->rotate_left( parent );
                                    std::swap( parent, cur );
                                }
                                // 第二种情况：先将parent和grandparent颜色互换然后右旋
                                parent->_color      = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                this->rotate_right( grandparent );
                            }
                        }
                        else {                                                      // 第二大类插入节点在红黑树的右子树：
                            auto uncle = grandparent->_left;                        // 当前节点的叔叔节点
                            if ( uncle && uncle->_color == Node::Color::RED ) {     // 第一种情况：叔叔节点存在而且为红色：
                                parent->_color      = Node::Color::BLACK;
                                uncle->_color       = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                cur                 = grandparent;
                                parent              = cur->_parent;
                            }
                            // 第二三种情况：
                            else {
                                // 因为我们要将第三种情况转化为第二种情况处理所以我们先写第三种情况：cur插在内侧
                                if ( cur == parent->_left ) {
                                    this->rotate_right( parent );
                                    std::swap( parent, cur );
                                }
                                // 第二种情况：先将parent和grandparent颜色互换然后右旋
                                parent->_color      = Node::Color::BLACK;
                                grandparent->_color = Node::Color::RED;
                                this->rotate_left( grandparent );
                            }
                        }
                    }
                }
            }
            _root->_color       = Node::Color::BLACK;
            this->_head->_left  = this->get_mostleftnode( );
            this->_head->_right = this->get_mostrightnode( );
            return TRUE;
        }

        void destroy( Node *&root ) {
            if ( root ) {
                this->destroy( root->_left );
                this->destroy( root->_right );
                root = nullptr;
            }
        }
        auto clear( ) {
            this->destroy( this->get_root( ) );
            this->_size = 0;
        }
        ~RBTree( ) {
            this->destroy( this->get_root( ) );
        }
        // 查找方法
        auto search( IN Keyofvalue key ) -> Node * {
            auto *cur = this->get_root( );
            while ( cur ) {
                if ( ( cur->_key ) < key ) {
                    cur = cur->_right;
                }
                else if ( ( cur->_key ) > key ) {
                    cur = cur->_left;
                }
                else {
                    return cur;
                }
            }
            return NULL;
        }
        auto swap( IN RBTree< T, Keyofvalue > _t ) {
            std::swap( _head, _t._head );
        }

        auto size( ) const {
            return _size;
        }
        auto empty( ) const {
            return !_size;
        }

        auto inoder( ) {
            auto *_root = this->get_root( );
            this->mid( _root );
        }

        auto is_RBtree( ) {
            auto *root = this->get_root( );
            if ( !root ) {
                return TRUE;
            }
            // 判断根节点是否是黑色节点：
            if ( root->_color == Node::Color::RED ) {
                return FALSE;
            }
            // 判断每条路径中黑色节点个数是否相同
            auto black_count = 0ul;
            auto cur         = root;
            while ( cur ) {
                if ( cur->_color == Node::Color::BLACK ) {
                    black_count++;
                }
                cur = cur->_left;
            }
            auto k = 0;
            return this->is_RBtree( black_count, k, root );
        }
        // 判断红黑树中是否满足性质三（两个红色节点不挨在一起）性质四（每条路径中黑色节点树相同）
        auto is_RBtree( size_t black_count, int k, Node *root ) {
            if ( !root ) {
                if ( k != black_count ) {
                    return FALSE;
                }
                return TRUE;
            }
            if ( root->_color == Node::Color::BLACK ) {
                k++;
            }
            else {
                if ( root->_parent->_color == Node::Color::RED ) {
                    return FALSE;
                }
            }
            return this->is_RBtree( black_count, k, root->_left ) && this->is_RBtree( black_count, k, root->_right );
        }

    private:
        // 中序遍历：
        auto mid( Node *root ) {
            if ( root ) {
                this->mid( root->_left );
                this->mid( root->_right );
            }
        }
        // 这里因为我们红黑树中没有设置根节点在代码实现的时候不容易理解所以这里我们写一个私有函数返回红黑树的根节点：
        auto *&get_root( ) {
            return _head->_parent;
        }
        // 获取最左侧节点也就是最小节点：
        auto get_mostleftnode( ) {
            auto *_root = get_root( );
            if ( _root ) {
                while ( _root->_left ) {
                    _root = _root->_left;
                }
            }
            return _root;
        }
        // 获取最右侧节点也就是最大节点：
        auto get_mostrightnode( ) {
            auto *_root = this->get_root( );
            if ( _root ) {
                while ( _root->_right ) {
                    _root = _root->_right;
                }
            }
            return _root;
        }
        // 左单旋
        auto rotate_left( Node *parent ) {
            auto pparent   = parent->_parent;
            auto subR      = parent->_right;
            auto subRL     = subR->_left;
            parent->_right = subRL;
            // 更新subRL的双亲：
            if ( subRL ) {
                subRL->_parent = parent;
            }
            subR->_left     = parent;
            parent->_parent = subR;
            subR->_parent   = pparent;
            if ( pparent == _head ) {
                _head->_parent = subR;
            }
            if ( pparent ) {
                if ( pparent->_left == parent ) {
                    pparent->_left = subR;
                }
                else {
                    pparent->_right = subR;
                }
            }
        }
        // 右单旋
        auto rotate_right( Node *parent ) {
            auto subL     = parent->_left;
            auto subLR    = subL->_right;
            auto pparent  = parent->_parent;
            parent->_left = subLR;
            // 如果subLR存在那么将其父节点更新
            if ( subLR ) {
                subLR->_parent = parent;
            }
            // 将parent右旋下来：
            subL->_right = parent;
            // parent旋下来就要更新parent的父节点
            parent->_parent = subL;
            // 此时subL就要更新父节点
            subL->_parent = pparent;
            if ( pparent == _head ) {
                _head->_parent = subL;
            }
            if ( pparent ) {
                if ( parent == pparent->_right ) {
                    pparent->_right = subL;
                }
                else {
                    pparent->_left = subL;
                }
            }
        }

    private:
        uint64_t _size;
        Node    *_head;
    };
}
