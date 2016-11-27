#pragma once

#include <stddef.h>

/**
 * \brief node of a linked_list
 */
struct ll_node
{
  void *data; /**< generic pointer containing the data */
  struct ll_node *next; /**< pointer to the next node */
};

/**
 * \brief Generic linked list head structure.
 */
struct linked_list
{
  struct ll_node *first; /**< Pointer to the first node of the list */
  struct ll_node *last; /**< Pointer to the last node of the list */
};

/**
 * \brief Allocate and return an empty linked_list
 *
 * \return A new linked_list.
 */
struct linked_list *list_make(void);

/**
 * \brief Push the data to a new node at the end of the list.
 *
 * \param ll  linked_list to push the data into
 * \param data  the data to push
 */
void list_add_back(struct linked_list *ll, void *data);

/**
 * \brief find the given data in the linked list. Undefined behavior if the data
 * is not found in the list.
 *
 * \param ll  linked_list to get the data from
 * \param compare_function  comparaison function used check the equality of
 * datas. The function need to return 0 if two elements matchs.
 *
 * \return the pointer to the data in the linked_list.
 */
void *list_extract(struct linked_list *ll,
                   const void *data,
                   int (* compare_function)(const void *, const void *));
/**
 * \brief remove the given node from the list. If the node is not in the linked
 * list, it's an undefined behavior.
 *
 * \param ll  linked_list to remove the node from
 * \param removed_node  the node to be removed
 * \param destruction  0 if the node/data should not be freed
 */
void list_remove(struct linked_list *ll,
                 struct ll_node *removed_node,
                 int destruction);
/**
 * \brief destroy a complete linked_list.
 *
 * \param ll  the linked_list to destroy
 * \param data_destruction  should be 0 if the data should not be freed.
 */
void list_destroy(struct linked_list *ll, int data_destruction);

/**
 * \brief helper macro to iterate on all elements of a linked_list
 *
 * \param pos  name of the iterator node.
 * \param ll  linked_list to iterate on.
 */
#define list_for_each(pos, ll) \
      struct ll_node *pos; \
      for (pos = ll->first; pos; pos = pos->next)
