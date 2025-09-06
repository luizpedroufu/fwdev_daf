#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utl_lst.h"

void utl_lst_init(struct utl_lst_node *head)
{
    head->next = head->prev = head;
}

/* Insert a new item between prev and next */
static void utl_lst_link(struct utl_lst_node *item, struct utl_lst_node *prev, struct utl_lst_node *next)
{
    next->prev = item;
    item->next = next;
    item->prev = prev;
    prev->next = item;
}

/* unlink elements between prev and next */
static void utl_lst_unlink(struct utl_lst_node *prev, struct utl_lst_node *next)
{
    next->prev = prev;
    prev->next = next;
}

void utl_lst_add(struct utl_lst_node *head, struct utl_lst_node *item, bool at_end)
{
    if(at_end)
    	utl_lst_link(item, head->prev, head);
    else
    	utl_lst_link(item, head, head->next);
}

/* check if the list is empty */
bool utl_lst_is_empty(const struct utl_lst_node *head)
{
    return head->next == head;
}

static void utl_lst_delete(struct utl_lst_node *entry)
{
    // already removed ???
    if(entry->next == 0 && entry->prev == 0)
        return;

    utl_lst_unlink(entry->prev, entry->next);
    entry->next = entry->prev = 0;
}
/* remove and return the first or last element */
struct utl_lst_node *utl_lst_pop(struct utl_lst_node *head, bool at_end)
{
     struct utl_lst_node *item = utl_lst_peek(head,at_end);

     if(item)
          utl_lst_delete(item);

     return item;
}
/* point to the first or last element */
struct utl_lst_node *utl_lst_peek(struct utl_lst_node *head, bool at_end)
{
     struct utl_lst_node *item = 0;

     if(!utl_lst_is_empty(head))
     {
         item = at_end ? head->prev : head->next;
     }

     return item;
}
