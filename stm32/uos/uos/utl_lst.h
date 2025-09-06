#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#define utl_lst_container_of(ptr, type, member) \
     ((type *)((char *)(ptr) - offsetof(type, member)))

struct utl_lst_node
{
    struct utl_lst_node *prev;
    struct utl_lst_node *next;
};

#define UTL_LST_INIT(list) { .prev = &(list), .next = &(list) }

void utl_lst_init(struct utl_lst_node *head);
void utl_lst_add(struct utl_lst_node *head, struct utl_lst_node *item, bool at_end);
bool utl_lst_is_empty(const struct utl_lst_node *head);
struct utl_lst_node *utl_lst_pop(struct utl_lst_node *head, bool at_end);
struct utl_lst_node *utl_lst_peek(struct utl_lst_node *head, bool at_end);

#ifdef __cplusplus
}
#endif
