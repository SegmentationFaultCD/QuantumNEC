#include <stdio.h>
#include <print>
// 自定义的结构体，用来存储链表的内容
struct ListNode
{
    ListNode *prev { }; /* 上一个节点 */
    ListNode *next { }; /* 下一个节点 */
    int container { };
    explicit ListNode( void ) noexcept = default;
    explicit ListNode( int a ) noexcept {
        container = a;
    }
    /**
     * @brief 判断上一节点十分为空
     * @param node 要判断的节点
     */
    auto is_empty_prev( ListNode *node ) -> ListNode * {
        return node->prev ? node->prev : NULL;
    }
    /**
     * @brief 判断下一节点十分为空
     * @param node 要判断的节点
     */
    auto is_empty_next( ListNode *node ) -> ListNode * {
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
struct ListTable
{
    ListNode head { }; /* 链表头 */
    ListNode end { };  /* 链表尾 */
public:
    ListTable( ) noexcept {
        this->head.prev = NULL;
        this->head.next = &this->end;
        this->end.prev = &this->head;
        this->end.next = NULL;
    }
    ~ListTable( ) noexcept = default;
    auto operator=( ListTable &lt ) -> ListTable & {
        this->head = lt.head;
        this->end = lt.end;
        return *this;
    }

public:
    auto begin( ) {
        return this->head.next;
    }
    /**
     * @brief 插入节点到链表末尾
     * @param New 要添加的元素的指针
     */
    auto append( ListNode &New ) {
        this->insert( &New, &this->end );
    }

    /**
     * @brief 插入节点到链表开头
     * @param New  要添加的元素的指针
     */
    auto push( ListNode &New ) {
        this->insert( &New, this->head.next );
    }
    /**
     * @brief 删除节点
     * @param entry 要弹出的节点
     */
    auto remove( ListNode &entry ) {
        entry.next->prev = entry.prev;
        entry.prev->next = entry.next;
    }
    /**
     * @brief 将链表第一个元素弹出并返回
     * @return 第一个元素
     */
    auto pop( ) {
        auto node { this->head.next };
        this->remove( *node );
        return node;
    }
    /**
     * @brief 判断链表是否为空
     */
    auto is_empty( ) {
        return this->head.next == &this->end ? true : false;
    }

    auto length( ) {
        unsigned long length { };
        for ( auto node { this->head.next }; node != &this->end; ++length ) {
            node = node->next;
        }
        return length;
    }
    auto insert( ListNode *node, ListNode *in_before ) -> void {
        in_before->prev->next = node;
        node->prev = in_before->prev;
        node->next = in_before;
        in_before->prev = node;
    };
    auto back( ) {
        auto node { this->end.prev };
        return node;
    }
};
ListTable lt { };
// 进行冒泡排序
void BubbleSort( ListNode *head ) {
    ListNode *p, *q, *tail;     // 创建三个指针，进行冒泡排序
    auto count = lt.length( );
    for ( auto i = 0ul; i < count - 1; i++ )     // 外层循环，跟数组冒泡排序一样
    {
        auto num = count - i - 1;     // 记录内层循环需要的次数，跟数组冒泡排序一样，
        q = head;                     // 令q指向第一个结点
        p = q->next;                  // 令p指向第二个结点
        tail = head->prev;            // 让tail始终指向q前一个结点，方便交换，也方便与进行下一步操作
        while ( num-- )               // 内层循环,次数跟数组冒泡排序一样
        {
            if ( q->container > p->container )     // 如果该结点的值大于后一个结点，则交换
            {
                q->next = p->next;
                p->next = q;
                tail->next = p;
            }
            // 进行指针的移动
            tail = tail->next;     // 注释②
            q = tail->next;        // 注释②
            p = q->next;           // 注释②
        }
    }
}

int main( ) {
    lt.append( *new ListNode { 2 } );
    lt.append( *new ListNode { 3 } );
    lt.append( *new ListNode { 1 } );
    lt.append( *new ListNode { 4 } );
    BubbleSort( lt.begin( ) );
    for ( auto i = lt.begin( ); i != lt.back( )->next; i = i->next ) {
        std::println( "{}", i->container );
    }
    return 0;
}