#include "triangle.h"
#include <stdbool.h>

static bool is_valid_triangle(triangle_t triangle) {
    // All sides must be positive
    if (triangle.a <= 0 || triangle.b <= 0 || triangle.c <= 0) {
        return false;
    }
    
    // Triangle inequality: sum of any two sides must be greater than the third
    if (triangle.a + triangle.b <= triangle.c ||
        triangle.a + triangle.c <= triangle.b ||
        triangle.b + triangle.c <= triangle.a) {
        return false;
    }
    
    return true;
}

bool is_equilateral(triangle_t triangle) {
    if (!is_valid_triangle(triangle)) {
        return false;
    }
    
    return triangle.a == triangle.b && triangle.b == triangle.c;
}

bool is_isosceles(triangle_t triangle) {
    if (!is_valid_triangle(triangle)) {
        return false;
    }
    
    return triangle.a == triangle.b || triangle.b == triangle.c || triangle.a == triangle.c;
}

bool is_scalene(triangle_t triangle) {
    if (!is_valid_triangle(triangle)) {
        return false;
    }
    
    return triangle.a != triangle.b && triangle.b != triangle.c && triangle.a != triangle.c;
}