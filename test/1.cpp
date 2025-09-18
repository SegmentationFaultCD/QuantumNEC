#include <print>
template < typename T >
struct alignas( 32 ) Message {
    std::uint64_t node;
    T *package_address;
    std::uint64_t length;
    std::uint64_t lock;
};
// 4K / 2^5 = 2^12 / 2^5 = 2^7 = 128

struct IPC {
    // classic productor-consumer problem
    // use shared memory
    // shared_page is a 4K memory page, and there is a message queue in it, it is composed of a array, structure be like:
    /* |      head     | => number of messages,
     * |   message1    | => address of package(physical address), length, lock, node(for the receiver's mailbox)
     * |   message2    |
     * - - - -
     * |   messageMax  |
     */
    // receiver process provides physical address of shared_page
    // any sender should map this address to thier page table
    // and write its request in the queue in order
    // when a sender tries to write request, it should check the message element's lock, if it is 0, lock and write.(atomic operation)
    // if not, search for the next element, do it like before. if all elements are locked, choose one randomly and block itself.
    // search algorithm is ???
    // in the end, add the request( = node = physical address of the message) into the process's mailbox
    // receiver will convert the address in its mailbox into special virtual address space and visit the message that it points.
    // then map the pack's physical address, and get the data, handle message.
    // after handling, free the lock, remove the request from the mailbox
    /*

        receiver --- mailbox |   phy_addr1   |   phy_addr2   |   phy_addr3   |
                     convert |   vir_addr1   |   vir_addr2   |   vir_addr3   |
                                    |               |               |
                                   \/              \/              \/
                             |   Message1    |   Message2    |   Message3    |
                                    |              |                |
                                   \/             \/               \/
                             | pack1_phy_addr| pack2_phy_addr| pack3_phy_addr|
                 map convert | pack1_vrt_addr| pack2_vrt_addr| pack3_vrt_addr|
                             | handle data   | handle data   | handle data   |

    */

    template < typename... Args >
    auto receive( std::uint64_t source ) {
        // check self mailbox
        // null, hang | not null, handle, wake up sender, continue handling or sleeping
        // sync
        // mailbox = get_current()->mailbox
    }

    auto send( std::uint64_t destination, auto &&...args ) {
    }
};
template < typename T, std::uint64_t MAXLEVEL >
    requires std::copyable< T >
class Skiplist {
public:
    template < class _T, class Ref, class Ptr >
    struct SkiplistIterator;
    struct Node {
        friend Skiplist;

        template < class _T, class Ref, class Ptr >
        friend struct SkiplistIterator;

        int64_t key;
        T data;

    public:
        Node *forwards[ MAXLEVEL ];
        using self = Node;

    public:
        Node( ) noexcept :
            data { }, key { }, forwards { } {
        }
        Node( const T &data, int64_t key ) noexcept :
            data { data }, key { key }, forwards { } {
        }
        auto &operator=( const self &node ) {
            this->data = node.data;
            this->key = node.key;
            for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
                this->forwards[ i ] = node.forwards[ i ];
            }
            return *this;
        }
        auto *operator->( ) {
            return &data;
        }
        auto &operator*( ) {
            return this->data;
        }
        auto &set_key( int64_t _key ) {
            this->key = _key;
            return *this;
        }
        auto &operator=( const T &data ) {
            this->data = data;
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
            return _pnode->data;
        }
        Ptr operator->( ) {
            return &_pnode->data;
        }
        // 迭代器加加:前置加加
        self operator++( ) {
            this->_pnode = this->_pnode->forwards[ 0 ];
            return *this;
        }
        self operator++( int ) {
            self temp = *this;
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
            return _pnode->forwards[ 0 ] != s._pnode;
        }
        auto is_empty( ) {
            return !this->_pnode;
        }
        operator bool( ) {
            return !this->_pnode;
        }
        Node *_pnode;
    };
    using iterator = SkiplistIterator< T, T &, T * >;
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
    auto insert( const T &data, std::int64_t key ) {
        Node *node = new Node { data, key };
        auto level_ = this->get_insert_level( );
        Node *s[ MAXLEVEL ] { };
        Node *current { }, *last { };
        for ( auto i = 0; i < MAXLEVEL; ++i ) {
            s[ i ] = head_.forwards[ i ];
        }

        current = last = &head_;

        for ( auto i = this->level - 1; i >= 0; i-- ) {
            while ( current->forwards[ i ]->key != std::numeric_limits< int64_t >::max( ) && node->key > current->forwards[ i ]->key ) {
                current = current->forwards[ i ];
            }
            s[ i ] = current;
        }

        last = current->forwards[ 0 ];

        if ( last && last->key == node->key ) {
            return;
        }
        if ( level_ > this->level ) {
            this->level = level_;
        }
        for ( auto i = 0l; i < this->level; i++ ) {
            node->forwards[ i ] = s[ i ]->forwards[ i ];
            s[ i ]->forwards[ i ] = node;
        }
        this->count++;
    }
    auto search( int64_t key ) {
        auto current = &head_;

        for ( auto i = level - 1; i >= 0; i-- ) {
            while ( current->forwards[ i ]->key != std::numeric_limits< int64_t >::max( ) && key > current->forwards[ i ]->key ) {
                current = current->forwards[ i ];
            }
        }

        current = current->forwards[ 0 ];
        if ( current && current->key == key ) {
            return iterator { current };
        }
        return iterator { nullptr };
    }
    template < typename OP >
        requires std::invocable< OP, const T & > && std::same_as< std::invoke_result_t< OP, const T & >, bool >
    auto traverse( OP &&operation ) {
        for ( auto i = this->level - 1; i >= 0; --i ) {
            auto q = this->head_.forwards[ i ];
            while ( q ) {
                if ( q->data && operation( *q->data ) ) {
                    return iterator { q };
                }
                q = q->forwards[ i ];
            }
        }
        return iterator { nullptr };
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

    Skiplist( ) noexcept :
        head_ { { }, std::numeric_limits< int64_t >::min( ) },
        first_ { { }, std::numeric_limits< int64_t >::min( ) },
        end_ { { }, std::numeric_limits< int64_t >::max( ) }, level { }, count { } {
        for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
            head_.forwards[ i ] = &first_;
            head_.forwards[ i ]->forwards[ i ] = &end_;
        }
    }
    ~Skiplist( ) {
    }

    auto is_empty( ) {
        return !this->count;
    }
    auto length( ) {
        return this->count;
    }

private:
    Node head_, first_, end_;
    uint64_t count;
    int64_t level;
    auto get_insert_level( ) {
        auto upcount = 0l;
        static auto _random = 1145ul;
        for ( auto i = 0ul; i < MAXLEVEL; ++i ) {
            auto num = ( _random * ( _random - 1 ) ) % MAXLEVEL;
            if ( num < 5 ) {
                upcount++;
            }
        }
        _random++;
        return upcount;
    }
};
#include <ranges>
auto main( void ) -> int {
    Skiplist< int *, 8 > sp;
    for ( auto i : std::ranges::views::iota( 1, 10 ) ) {
        sp.insert( new int { i }, i );
    }
    for ( auto j : sp ) {
        std::println( "{}", *j );
    }
    sp.insert( new int { 1145 }, 114 );
    std::println( "{}", **sp.search( 114 ) );

    std::println( "{:x}", 18446744073709551615ull );
}