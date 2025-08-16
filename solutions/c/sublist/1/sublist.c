#include "sublist.h"
#include <stdbool.h>

// Helper function to check if haystack contains needle starting at given position
static bool contains_at_position(int *needle, int *haystack, 
                                size_t needle_len, size_t haystack_len, 
                                size_t start_pos) {
    if (start_pos + needle_len > haystack_len) {
        return false;
    }
    
    for (size_t i = 0; i < needle_len; i++) {
        if (needle[i] != haystack[start_pos + i]) {
            return false;
        }
    }
    return true;
}

// Helper function to check if haystack contains needle as a contiguous sublist
static bool contains_sublist(int *needle, int *haystack, 
                            size_t needle_len, size_t haystack_len) {
    // Empty list is contained in any list
    if (needle_len == 0) {
        return true;
    }
    
    // If needle is longer than haystack, it can't be contained
    if (needle_len > haystack_len) {
        return false;
    }
    
    // Check all possible starting positions
    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        if (contains_at_position(needle, haystack, needle_len, haystack_len, i)) {
            return true;
        }
    }
    return false;
}

comparison_result_t check_lists(int *list_to_compare, int *base_list,
                                size_t list_to_compare_element_count,
                                size_t base_list_element_count) {
    
    // Check if lists are equal
    if (list_to_compare_element_count == base_list_element_count) {
        bool equal = true;
        for (size_t i = 0; i < list_to_compare_element_count; i++) {
            if (list_to_compare[i] != base_list[i]) {
                equal = false;
                break;
            }
        }
        if (equal) {
            return EQUAL;
        }
    }
    
    // Check if list_to_compare is a sublist of base_list
    if (contains_sublist(list_to_compare, base_list, 
                        list_to_compare_element_count, base_list_element_count)) {
        return SUBLIST;
    }
    
    // Check if list_to_compare is a superlist of base_list
    if (contains_sublist(base_list, list_to_compare, 
                        base_list_element_count, list_to_compare_element_count)) {
        return SUPERLIST;
    }
    
    return UNEQUAL;
}
