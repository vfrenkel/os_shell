// TODO: add header here.

#include <stdlib.h>

#include "sllist.h"


struct Node *add_front(struct SLList *list, void *data) {
  struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
  new_node->data = data;
  new_node->next = NULL;

  if (list->head == NULL) {
    list->head = new_node;
    list->tail_node = new_node;
  } else {
    new_node->next = list->head;
    list->head = new_node;
  }

  return new_node;
}

struct Node *add_back(struct SLList *list, void *data) {
  struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
  new_node->data = data;
  new_node->next = NULL;

  if (list->tail_node == NULL) {
    list->head = new_node;
    list->tail_node = new_node;
  } else {
    list->tail_node->next = new_node;
    list->tail_node = new_node;
  }

  return new_node;
}

void traverse(struct SLList *list, void (*f)(void*)) {
  struct Node *curr_node = list->head;
  while (curr_node != NULL) {
    (*f)(curr_node->data);
    curr_node = curr_node->next;
  }
}

void *pop_front(struct SLList *list) {
  if (list->head != NULL) {
    struct Node *new_head = list->head->next;
    void *front_data = list->head->data;
    free(list->head);
    list->head = new_head;
    return front_data;
  } else {
    return NULL;
  }
}

