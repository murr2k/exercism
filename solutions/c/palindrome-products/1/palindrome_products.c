#include "palindrome_products.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Helper function to check if a number is a palindrome
static bool is_palindrome(int num) {
    int original = num;
    int reversed = 0;
    
    while (num > 0) {
        reversed = reversed * 10 + num % 10;
        num /= 10;
    }
    
    return original == reversed;
}

// Helper function to add a factor to the linked list
static factor_t* add_factor(factor_t* head, int a, int b) {
    factor_t* new_factor = malloc(sizeof(factor_t));
    if (!new_factor) return head;
    
    new_factor->factor_a = a;
    new_factor->factor_b = b;
    new_factor->next = head;
    
    return new_factor;
}

// Helper function to free factors list
static void free_factors(factor_t* factors) {
    while (factors) {
        factor_t* temp = factors;
        factors = factors->next;
        free(temp);
    }
}

product_t *get_palindrome_product(int from, int to) {
    product_t* product = malloc(sizeof(product_t));
    if (!product) return NULL;
    
    // Initialize the product
    product->smallest = 0;
    product->largest = 0;
    product->factors_sm = NULL;
    product->factors_lg = NULL;
    memset(product->error, 0, MAXERR);
    
    // Check for invalid input
    if (from > to) {
        snprintf(product->error, MAXERR, "invalid input: min is %d and max is %d", from, to);
        return product;
    }
    
    int smallest_palindrome = 0;
    int largest_palindrome = 0;
    factor_t* smallest_factors = NULL;
    factor_t* largest_factors = NULL;
    
    // Find all palindromes in the range
    for (int i = from; i <= to; i++) {
        for (int j = i; j <= to; j++) {
            int product_val = i * j;
            
            if (is_palindrome(product_val)) {
                if (smallest_palindrome == 0 || product_val < smallest_palindrome) {
                    smallest_palindrome = product_val;
                    free_factors(smallest_factors);
                    smallest_factors = add_factor(NULL, i, j);
                } else if (product_val == smallest_palindrome) {
                    smallest_factors = add_factor(smallest_factors, i, j);
                }
                
                if (largest_palindrome == 0 || product_val > largest_palindrome) {
                    largest_palindrome = product_val;
                    free_factors(largest_factors);
                    largest_factors = add_factor(NULL, i, j);
                } else if (product_val == largest_palindrome) {
                    largest_factors = add_factor(largest_factors, i, j);
                }
            }
        }
    }
    
    if (smallest_palindrome == 0) {
        snprintf(product->error, MAXERR, "no palindrome with factors in the range %d to %d", from, to);
    } else {
        product->smallest = smallest_palindrome;
        product->largest = largest_palindrome;
        product->factors_sm = smallest_factors;
        product->factors_lg = largest_factors;
    }
    
    return product;
}

void free_product(product_t *p) {
    if (p) {
        free_factors(p->factors_sm);
        free_factors(p->factors_lg);
        free(p);
    }
}
