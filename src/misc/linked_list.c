#include "linked_list.h"

#include <assert.h>
#include <stdlib.h>

/**
 * \brief linked_list internal function to create a node.
 *
 * \param data  the initial data to put in the node.
 *
 * \return the created node.
 */
static struct ll_node *node_make(void *data)
{
  assert(data);

  struct ll_node *node = malloc(sizeof(struct ll_node));
  if (!node)
    return NULL;

  node->data = data;
  node->next = NULL;

  return node;
}

/**
 * \brief linked_list internal function to free a node.
 *
 * \param node  the node to be destroyed
 * \param data_destruction  should be 0 if the data should not be freed.
 */
static void node_destroy(struct ll_node *node, int data_destruction)
{
  if (!node)
    return;

  node_destroy(node->next, data_destruction);
  if (data_destruction)
    free(node->data);
  free(node);
}

/**
 * \brief linked_list internal helper function to instantiate the first node
 * in a linked_list. If the list is not empty, an undefined behavior occures.
 *
 * \param ll  linked_list in which the data need to be inserted.
 * \param data  the data to be inserted.
 */
static void first_insertion(struct linked_list *ll, void *data)
{
  assert(ll->first == NULL && ll->last == NULL);

  ll->first = node_make(data);
  ll->last = ll->first;
}

struct linked_list *list_make(void)
{
  return calloc(1, sizeof(struct linked_list));
}

void list_add_back(struct linked_list *ll, void *data)
{
  assert(ll);
  assert(data && "Insertion of NULL data is not allowed!");

  if (ll->first == NULL)
  {
    first_insertion(ll, data);

    return;
  }

  ll->last->next = node_make(data);
  ll->last = ll->last->next;
}

void *list_extract(struct linked_list *ll,
                   const void *data,
                   int (* compare_function)(const void *, const void *))
{
  assert(ll);
  assert(data && "Data can't be NULL.");

  struct ll_node *ptr = ll->first;
  while (ptr && compare_function(ptr->data, data))
    ptr = ptr->next;

  if (!ptr)
    return NULL;

  return ptr->data;
}

void list_remove(struct linked_list *ll,
                 struct ll_node *removed_node,
                 int destruction)
{
  assert(ll && removed_node);

  if (ll->first == removed_node)
  {
    ll->first = removed_node->next;
    if (ll->last == removed_node)
      ll->last = NULL; // single node case
  }
  else
  {
    struct ll_node *ptr = ll->first;
    while (ptr && ptr->next != removed_node)
      ptr = ptr->next;

    if (!ptr)
      return; // the node is not in the list

    ptr->next = removed_node->next;
  }

  if (destruction)
  {
    free(removed_node->data);
    free(removed_node);
  }
}

void list_destroy(struct linked_list *ll, int data_destruction)
{
  assert(ll);

  node_destroy(ll->first, data_destruction);
  free(ll);
}
