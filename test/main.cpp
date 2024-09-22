#include <print>
struct alignas( 128 ) PageInformation
{
    uint64_t group_node[ 3 ];     // 连接每个页头
    struct PageFlags
    {
        uint64_t state : 2;
        uint64_t type : 2;
        uint64_t physical_base : 60;

        explicit PageFlags( void ) noexcept = default;

    } flags;     // 标志

    uint64_t map_base_adderess;     // 记录块起始地址
    uint64_t all_memory_page_count;
    uint64_t free_memory_page_count;

    uint64_t *bitmap_;
    PageInformation *next;
    explicit PageInformation( void ) = default;
};
auto main( int, char ** ) -> int {
    long prio_ratios[ 103 ] { };
    std::print( "{}", sizeof( PageInformation ) );
}
