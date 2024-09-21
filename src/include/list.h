#ifndef _LIST_H_
#define _LIST_H_
#include "include/stdint.h"
#include "include/stdio.h"
#include "include/stdbool.h"
namespace Kernel
{
	typedef struct
	{
		List* prev;
		List* next;
	}List; // 链表数据

	/*  */
	inline VOID list_init(List* list);
	/*  */
	inline VOID list_add_to_behind(List* entry, List* New);
	/*  */
	inline VOID list_add_to_before(List* entry, List* New);
	/*  */
	inline VOID list_del(List* entry);
	/*  */
	inline BOOL list_is_empty(List* entry);
	/*  */
	inline List* list_prev(List* entry);
	/*  */
	inline List* list_next(List* entry);
}
#endif /* _LIST_H_ */