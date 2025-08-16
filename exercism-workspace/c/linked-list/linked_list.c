#include "linked_list.h"
#include <stdlib.h>

struct list_node {
   struct list_node *prev, *next;
   ll_data_t data;
};

struct list {
   struct list_node *first, *last;
};

struct list *list_create(void)
{
    struct list *new_list = malloc(sizeof(struct list));
    if (new_list) {
        new_list->first = NULL;
        new_list->last = NULL;
    }
    return new_list;
}

size_t list_count(const struct list *list)
{
    if (!list) return 0;
    
    size_t count = 0;
    struct list_node *current = list->first;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

void list_push(struct list *list, ll_data_t item_data)
{
    if (!list) return;
    
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;
    
    new_node->data = item_data;
    new_node->next = NULL;
    new_node->prev = list->last;
    
    if (list->last) {
        list->last->next = new_node;
    } else {
        list->first = new_node;
    }
    list->last = new_node;
}

ll_data_t list_pop(struct list *list)
{
    if (!list || !list->last) return 0; // Undefined behavior in tests, but return 0
    
    struct list_node *node_to_remove = list->last;
    ll_data_t data = node_to_remove->data;
    
    list->last = node_to_remove->prev;
    if (list->last) {
        list->last->next = NULL;
    } else {
        list->first = NULL;
    }
    
    free(node_to_remove);
    return data;
}

void list_unshift(struct list *list, ll_data_t item_data)
{
    if (!list) return;
    
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;
    
    new_node->data = item_data;
    new_node->prev = NULL;
    new_node->next = list->first;
    
    if (list->first) {
        list->first->prev = new_node;
    } else {
        list->last = new_node;
    }
    list->first = new_node;
}

ll_data_t list_shift(struct list *list)
{
    if (!list || !list->first) return 0; // Undefined behavior in tests, but return 0
    
    struct list_node *node_to_remove = list->first;
    ll_data_t data = node_to_remove->data;
    
    list->first = node_to_remove->next;
    if (list->first) {
        list->first->prev = NULL;
    } else {
        list->last = NULL;
    }
    
    free(node_to_remove);
    return data;
}

void list_delete(struct list *list, ll_data_t data)
{
    if (!list) return;
    
    struct list_node *current = list->first;
    while (current) {
        if (current->data == data) {
            // Remove this node
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                list->first = current->next;
            }
            
            if (current->next) {
                current->next->prev = current->prev;
            } else {
                list->last = current->prev;
            }
            
            free(current);
            return; // Only delete first occurrence
        }
        current = current->next;
    }
}

void list_destroy(struct list *list)
{
    if (!list) return;
    
    struct list_node *current = list->first;
    while (current) {
        struct list_node *next = current->next;
        free(current);
        current = next;
    }
    
    free(list);
}
