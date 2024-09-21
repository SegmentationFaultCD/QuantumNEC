#include "include/list.h"
namespace Kernel
{
	inline VOID list_init(List* list)
	{
		list->prev = list;
		list->next = list;
	}

	inline VOID list_add_to_behind(List* entry, List* New)
	{
		New->next = entry->next;
		New->prev = entry;
		New->next->prev = New;
		entry->next = New;
	}

	inline VOID list_add_to_before(List* entry, List* New)
	{
		New->next = entry;
		entry->prev->next = New;
		New->prev = entry->prev;
		entry->prev = New;
	}

	inline VOID list_del(List* entry)
	{
		entry->next->prev = entry->prev;
		entry->prev->next = entry->next;
	}

	inline BOOL list_is_empty(List* entry)
	{
		if(entry == entry->next && entry->prev == entry)
			return true;
		else
			return false;
	}

	inline List* list_prev(List* entry)
	{
		if(entry->prev != nullptr)
			return entry->prev;
		else
			return nullptr;
	}

	inline List* list_next(List* entry)
	{
		if(entry->next != nullptr)
			return entry->next;
		else
			return nullptr;
	}
}