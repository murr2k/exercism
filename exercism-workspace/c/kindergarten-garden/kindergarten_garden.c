#include "kindergarten_garden.h"
#include <string.h>

// Convert plant character to plant_t enum
static plant_t char_to_plant(char c) {
    switch (c) {
        case 'C': return CLOVER;
        case 'G': return GRASS;
        case 'R': return RADISHES;
        case 'V': return VIOLETS;
        default: return CLOVER; // Should not happen with valid input
    }
}

// Get student index based on name (Alice=0, Bob=1, etc.)
static int get_student_index(const char *student) {
    const char *students[] = {
        "Alice", "Bob", "Charlie", "David", "Eve", "Fred",
        "Ginny", "Harriet", "Ileana", "Joseph", "Kincaid", "Larry"
    };
    
    for (int i = 0; i < 12; i++) {
        if (strcmp(student, students[i]) == 0) {
            return i;
        }
    }
    
    return -1; // Student not found
}

plants_t plants(const char *diagram, const char *student) {
    plants_t result = { .plants = {0} };
    
    int student_index = get_student_index(student);
    if (student_index == -1) {
        return result; // Invalid student
    }
    
    // Each student gets 2 consecutive plants, so student index * 2 gives starting position
    int start_pos = student_index * 2;
    
    // Find the newline to separate the two rows
    const char *newline = strchr(diagram, '\n');
    if (!newline) {
        return result; // Invalid diagram format
    }
    
    // First row (plants[0] and plants[1])
    if (start_pos < (newline - diagram) && start_pos + 1 < (newline - diagram)) {
        result.plants[0] = char_to_plant(diagram[start_pos]);
        result.plants[1] = char_to_plant(diagram[start_pos + 1]);
    }
    
    // Second row (plants[2] and plants[3])
    const char *second_row = newline + 1;
    int second_row_len = strlen(second_row);
    if (start_pos < second_row_len && start_pos + 1 < second_row_len) {
        result.plants[2] = char_to_plant(second_row[start_pos]);
        result.plants[3] = char_to_plant(second_row[start_pos + 1]);
    }
    
    return result;
}
