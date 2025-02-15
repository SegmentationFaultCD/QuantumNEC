#include <cstdint>
#include <numeric>
#include <print>
#include <ranges>
template < typename T >
class Skiplist {
    static constexpr auto MAXLEVEL = 12;

public:
    template < class _T, class Ref, class Ptr >
    struct SkiplistIterator;
    class Node {
        friend Skiplist;

        template < class _T, class Ref, class Ptr >
        friend struct SkiplistIterator;

    private:
        int64_t key;
        T      *data;

    public:
        Node *forwards[ MAXLEVEL ];
        using self = Node;

    public:
        Node( ) noexcept :
            data { nullptr }, key { }, forwards { } {
        }
        Node( T *data, int64_t key ) noexcept :
            data { data }, key { key }, forwards { } {
        }
        auto &operator=( const self &node ) {
            this->data = node.data;
            this->key  = node.key;
            for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
                this->forwards[ i ] = node.forwards[ i ];
            }
            return *this;
        }
        auto operator->( ) {
            return data;
        }
        auto &operator*( ) {
            return *this->data;
        }

        auto &operator=( T &data ) {
            this->data = &data;
            return *this;
        }
    };

    template < class _T, class Ref, class Ptr >
    struct SkiplistIterator {
        using self = SkiplistIterator< _T, Ref, Ptr >;
        SkiplistIterator( Node *node = nullptr ) :
            _pnode { node } {
        }
        // 迭代器解引用：
        Ref operator*( ) {
            return *_pnode->data;
        }
        Ptr operator->( ) {
            return _pnode->data;
        }
        // 迭代器加加:前置加加
        self operator++( ) {
            this->_pnode = this->_pnode->forwards[ 0 ];
            return *this;
        }
        self operator++( int ) {
            self temp    = *this;
            this->_pnode = this->_pnode->forwards[ 0 ];
            return temp;
        }
        self operator--( ) {
            return *this;
        }
        self operator--( int ) {
            self temp = *this;

            return temp;
        }

        bool operator==( const self &s ) const {
            return _pnode == s._pnode;
        }
        bool operator!=( const self &s ) const {
            return _pnode->forwards[ 0 ];
        }
        Node *_pnode;
    };
    using iterator       = SkiplistIterator< T, T &, T * >;
    using const_iterator = const SkiplistIterator< T, T &, T * >;
    auto begin( ) {
        return iterator { head_.forwards[ 0 ]->forwards[ 0 ] };
    }
    auto end( ) {
        return iterator { nullptr };
    }
    auto begin( ) const {
        return const_iterator { head_.forwards[ 0 ]->forwards[ 0 ] };
    }
    auto end( ) const {
        return const_iterator { nullptr };
    }

public:
    auto insert( Node &node ) {
        auto level_ = this->get_insert_level( );

        Node *s[ MAXLEVEL ] { };
        Node *current { }, *last { };
        for ( auto i = 0; i < MAXLEVEL; ++i ) {
            s[ i ] = head_.forwards[ i ];
        }

        current = last = &head_;

        for ( auto i = this->level - 1; i >= 0; i-- ) {
            while ( current->forwards[ i ]->key != std::numeric_limits< int64_t >::max( ) && node.key > current->forwards[ i ]->key ) {
                current = current->forwards[ i ];
            }
            s[ i ] = current;
        }

        last = current->forwards[ 0 ];
        if ( last && last->key == node.key ) {
            return;
        }
        if ( level_ > this->level ) {
            this->level = level_;
        }
        for ( auto i = 0l; i < this->level; i++ ) {
            node.forwards[ i ]    = s[ i ]->forwards[ i ];
            s[ i ]->forwards[ i ] = &node;
        }
        this->count++;
    }
    auto search( int64_t key ) -> Node * {
        auto current = &head_;

        for ( auto i = level - 1; i >= 0; i-- ) {
            while ( current->forwards[ i ]->key != std::numeric_limits< int64_t >::max( ) && key > current->forwards[ i ]->key ) {
                current = current->forwards[ i ];
            }
        }

        current = current->forwards[ 0 ];
        if ( current && current->key == key ) {
            return current;
        }
        return nullptr;
    }

    auto remove( int64_t key ) {
        Node *s[ MAXLEVEL ] { };
        Node *current { }, *last { };
        for ( auto i = 0; i < MAXLEVEL; ++i ) {
            s[ i ] = head_.forwards[ i ];
        }
        current = last = &head_;

        for ( auto i = this->level - 1; i >= 0; i-- ) {
            while ( current->forwards[ i ]->key != std::numeric_limits< int64_t >::max( ) && key > current->forwards[ i ]->key ) {
                current = current->forwards[ i ];
            }
            s[ i ] = current;
        }
        last = current->forwards[ 0 ];
        if ( last->key != key ) {
            return;
        }
        for ( auto i = 0l; i < this->level; i++ ) {
            s[ i ]->forwards[ i ] = s[ i ]->forwards[ i ]->forwards[ i ];
        }
        this->count--;
    }
    auto remove( Node &node ) {
        this->remove( node.key );
    }
    Skiplist( ) noexcept :
        head_ { nullptr, std::numeric_limits< int64_t >::min( ) },
        first_ { nullptr, std::numeric_limits< int64_t >::min( ) },
        end_ { nullptr, std::numeric_limits< int64_t >::max( ) }, level { }, count { } {
        for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
            head_.forwards[ i ]                = &first_;
            head_.forwards[ i ]->forwards[ i ] = &end_;
        }
    }
    auto init( ) {
        this->head_.data  = nullptr;
        this->head_.key   = std::numeric_limits< int64_t >::min( );
        this->first_.data = nullptr;
        this->first_.key  = std::numeric_limits< int64_t >::min( );
        this->end_.data   = nullptr;
        this->end_.key    = std::numeric_limits< int64_t >::max( );
        this->level       = 0;
        this->count       = 0;

        for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
            head_.forwards[ i ]                = &first_;
            head_.forwards[ i ]->forwards[ i ] = &end_;
        }
    }
    auto is_empty( ) {
        return !this->count;
    }
    auto length( ) {
        return this->count;
    }

private:
    Node     head_, first_, end_;
    uint64_t count;
    int64_t  level;
    auto     get_insert_level( ) {
        auto        upcount = 0l;
        static auto i       = 1145;
        // 获取系统时间 jiffies
        //
        for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
            auto num = ( i * ( i - 1 ) ) % MAXLEVEL;
            if ( num < 5 ) {
                upcount++;
            }
        }
        i++;
        return upcount;
    }
};

auto main( void ) -> int {
    Skiplist< uint64_t > l;
    for ( uint64_t i : std::ranges::views::iota( 0, 7 ) ) {
        l.insert( *new Skiplist< uint64_t >::Node { new uint64_t { i }, (int64_t)i } );
        std::print( "{} ", i );
    }
    std::println( );

    for ( auto &n : l ) {
        std::print( "{} ", n );
    }
    std::println( );
    l.remove( 0 );
    for ( auto &n : l ) {
        std::print( "{} ", n );
    }
    std::println( );
    l.remove( 5 );
    for ( auto &n : l ) {
        std::print( "{} ", n );
    }
    std::println( );
}