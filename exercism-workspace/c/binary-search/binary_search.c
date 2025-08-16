#include "binary_search.h"

const int *binary_search(int value, const int *arr, size_t length)
{
    if (arr == NULL || length == 0) {
        return NULL;
    }
    
    size_t left = 0;
    size_t right = length - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        
        if (arr[mid] == value) {
            return &arr[mid];
        } else if (arr[mid] < value) {
            left = mid + 1;
        } else {
            if (mid == 0) {
                break; // Prevent underflow when mid is 0
            }
            right = mid - 1;
        }
    }
    
    return NULL;
}
