#pragma once

#include <stddef.h>

struct ll_node
{
  void *data;
  struct ll_node *next;
};

struct linked_list
{
  struct ll_node *first;
  struct ll_node *last;
};

struct linked_list *list_make(void);
void list_add_back(struct linked_list *ll, void *data);
size_t list_size(struct linked_list *ll);
void *list_extract(struct linked_list *ll,
                   const void *data,
                   int (* compare_function)(const void *, const void *));
void list_remove(struct linked_list *ll,
                 struct ll_node *removed_node,
                 int destruction);
void list_destroy(struct linked_list *ll, int data_destruction);

#define list_for_each(pos, ll) \
      struct ll_node *pos; \
      for (pos = ll->first; pos; pos = pos->next)
