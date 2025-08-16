#include "rational_numbers.h"
#include <math.h>
#include <stdlib.h>

// Helper function to compute GCD
static int16_t gcd(int16_t a, int16_t b) {
    a = abs(a);
    b = abs(b);
    while (b != 0) {
        int16_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Helper function to reduce a rational number to lowest terms
rational_t reduce(rational_t r) {
    if (r.numerator == 0) {
        return (rational_t){0, 1};
    }
    
    int16_t g = gcd(r.numerator, r.denominator);
    r.numerator /= g;
    r.denominator /= g;
    
    // Ensure denominator is positive
    if (r.denominator < 0) {
        r.numerator = -r.numerator;
        r.denominator = -r.denominator;
    }
    
    return r;
}

rational_t add(rational_t r1, rational_t r2) {
    rational_t result;
    result.numerator = r1.numerator * r2.denominator + r2.numerator * r1.denominator;
    result.denominator = r1.denominator * r2.denominator;
    return reduce(result);
}

rational_t subtract(rational_t r1, rational_t r2) {
    rational_t result;
    result.numerator = r1.numerator * r2.denominator - r2.numerator * r1.denominator;
    result.denominator = r1.denominator * r2.denominator;
    return reduce(result);
}

rational_t multiply(rational_t r1, rational_t r2) {
    rational_t result;
    result.numerator = r1.numerator * r2.numerator;
    result.denominator = r1.denominator * r2.denominator;
    return reduce(result);
}

rational_t divide(rational_t r1, rational_t r2) {
    rational_t result;
    result.numerator = r1.numerator * r2.denominator;
    result.denominator = r1.denominator * r2.numerator;
    return reduce(result);
}

rational_t absolute(rational_t r) {
    rational_t result = r;
    if (result.numerator < 0) {
        result.numerator = -result.numerator;
    }
    if (result.denominator < 0) {
        result.denominator = -result.denominator;
    }
    return reduce(result);
}

rational_t exp_rational(rational_t r, int16_t n) {
    if (n == 0) {
        return (rational_t){1, 1};
    }
    
    if (n > 0) {
        rational_t result = {1, 1};
        for (int16_t i = 0; i < n; i++) {
            result = multiply(result, r);
        }
        return result;
    } else {
        // n < 0, so we compute r^(-|n|) = (1/r)^|n|
        rational_t reciprocal = {r.denominator, r.numerator};
        rational_t result = {1, 1};
        for (int16_t i = 0; i < -n; i++) {
            result = multiply(result, reciprocal);
        }
        return result;
    }
}

float exp_real(float x, rational_t r) {
    // x^(a/b) = (x^a)^(1/b) = b-th root of x^a
    // We can compute this as pow(x, (float)a/b)
    float rational_as_float = (float)r.numerator / (float)r.denominator;
    return powf(x, rational_as_float);
}
