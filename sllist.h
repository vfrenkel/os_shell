/*
 * sllist.h
 * An implementation of a singly linked list.
 */

#ifndef __SLLIST_H_
#define __SLLIST_H_

struct Node {
  void *data;
  struct Node *next;
};

/*
 * Maintain a pointer to head node, but also to
 * the last node in the list: useful for constant
 * time operations on the tail end of the list.
 */
struct SLList {
  struct Node *head;
  struct Node *tail_node;
};

/*
 * Initialize the empty list.
 */
static inline void init_list(struct SLList *list) {
  list->head = NULL;
  list->tail_node = NULL;
}

/*
 * Returns true if the list is empty, returns 
 * false otherwise.
 */
static inline int is_empty(struct SLList *list) {
  return (list->head == NULL);
}

/*
 * Create a node that holds the provided data pointer,
 * and add this node to the front of the list.
 */
struct Node *add_front(struct SLList *list, void *data);

/*
 * Create a node that holds the provided data pointer,
 * and add this node to the back of the list.
 */
struct Node *add_back(struct SLList *list, void *data);

/*
 * Traverse the list, calling the 
 * function f on each data item.
 */
void traverse(struct SLList *list, void (*f)(void *));

/*
 * Remove the first node from the list, deallocate
 * the memory used by the node, and return the node's
 * data pointer. Returns NULL if the list is empty.
 */
void *pop_front(struct SLList *list);


#endif //__SLLIST_H_
