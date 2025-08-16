#include "matching_brackets.h"
#include <string.h>

#define MAX_STACK_SIZE 1000

static bool is_opening_bracket(char c)
{
    return c == '(' || c == '[' || c == '{';
}

static bool is_closing_bracket(char c)
{
    return c == ')' || c == ']' || c == '}';
}

static bool brackets_match(char opening, char closing)
{
    return (opening == '(' && closing == ')') ||
           (opening == '[' && closing == ']') ||
           (opening == '{' && closing == '}');
}

bool is_paired(const char *input)
{
    if (input == NULL) {
        return true;
    }
    
    char stack[MAX_STACK_SIZE];
    int stack_top = -1;  // Stack is empty when stack_top == -1
    
    for (size_t i = 0; i < strlen(input); i++) {
        char c = input[i];
        
        if (is_opening_bracket(c)) {
            // Push opening bracket onto stack
            if (stack_top >= MAX_STACK_SIZE - 1) {
                return false;  // Stack overflow
            }
            stack[++stack_top] = c;
        } else if (is_closing_bracket(c)) {
            // Check if closing bracket matches top of stack
            if (stack_top == -1) {
                return false;  // No opening bracket to match
            }
            
            char opening = stack[stack_top--];
            if (!brackets_match(opening, c)) {
                return false;  // Mismatched brackets
            }
        }
        // Ignore all other characters
    }
    
    // All brackets should be paired (stack should be empty)
    return stack_top == -1;
}
