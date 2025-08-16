#include "list_ops.h"
#include <string.h>

list_t *new_list(size_t length, list_element_t elements[]) {
    list_t *list = malloc(sizeof(list_t) + length * sizeof(list_element_t));
    if (!list) return NULL;
    
    list->length = length;
    if (length > 0 && elements != NULL) {
        memcpy(list->elements, elements, length * sizeof(list_element_t));
    }
    
    return list;
}

list_t *append_list(list_t *list1, list_t *list2) {
    if (!list1 || !list2) return NULL;
    
    size_t new_length = list1->length + list2->length;
    list_t *result = malloc(sizeof(list_t) + new_length * sizeof(list_element_t));
    if (!result) return NULL;
    
    result->length = new_length;
    
    // Copy elements from list1
    if (list1->length > 0) {
        memcpy(result->elements, list1->elements, list1->length * sizeof(list_element_t));
    }
    
    // Copy elements from list2
    if (list2->length > 0) {
        memcpy(result->elements + list1->length, list2->elements, list2->length * sizeof(list_element_t));
    }
    
    return result;
}

list_t *filter_list(list_t *list, bool (*filter)(list_element_t)) {
    if (!list || !filter) return NULL;
    
    // First pass: count how many elements will pass the filter
    size_t count = 0;
    for (size_t i = 0; i < list->length; i++) {
        if (filter(list->elements[i])) {
            count++;
        }
    }
    
    // Create new list with the counted size
    list_t *result = malloc(sizeof(list_t) + count * sizeof(list_element_t));
    if (!result) return NULL;
    
    result->length = count;
    
    // Second pass: copy elements that pass the filter
    size_t index = 0;
    for (size_t i = 0; i < list->length; i++) {
        if (filter(list->elements[i])) {
            result->elements[index++] = list->elements[i];
        }
    }
    
    return result;
}

size_t length_list(list_t *list) {
    return list ? list->length : 0;
}

list_t *map_list(list_t *list, list_element_t (*map)(list_element_t)) {
    if (!list || !map) return NULL;
    
    list_t *result = malloc(sizeof(list_t) + list->length * sizeof(list_element_t));
    if (!result) return NULL;
    
    result->length = list->length;
    
    for (size_t i = 0; i < list->length; i++) {
        result->elements[i] = map(list->elements[i]);
    }
    
    return result;
}

list_element_t foldl_list(list_t *list, list_element_t initial,
                          list_element_t (*foldl)(list_element_t, list_element_t)) {
    if (!list || !foldl) return initial;
    
    list_element_t accumulator = initial;
    for (size_t i = 0; i < list->length; i++) {
        accumulator = foldl(list->elements[i], accumulator);
    }
    
    return accumulator;
}

list_element_t foldr_list(list_t *list, list_element_t initial,
                          list_element_t (*foldr)(list_element_t, list_element_t)) {
    if (!list || !foldr) return initial;
    
    list_element_t accumulator = initial;
    for (size_t i = list->length; i > 0; i--) {
        accumulator = foldr(list->elements[i - 1], accumulator);
    }
    
    return accumulator;
}

list_t *reverse_list(list_t *list) {
    if (!list) return NULL;
    
    list_t *result = malloc(sizeof(list_t) + list->length * sizeof(list_element_t));
    if (!result) return NULL;
    
    result->length = list->length;
    
    for (size_t i = 0; i < list->length; i++) {
        result->elements[i] = list->elements[list->length - 1 - i];
    }
    
    return result;
}

void delete_list(list_t *list) {
    if (list) {
        free(list);
    }
}
