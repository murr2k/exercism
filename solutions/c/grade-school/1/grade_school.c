#include "grade_school.h"
#include <string.h>
#include <stdbool.h>

void init_roster(roster_t *roster)
{
    if (roster != NULL) {
        roster->count = 0;
    }
}

static bool student_exists(roster_t *roster, const char *name)
{
    for (size_t i = 0; i < roster->count; i++) {
        if (strcmp(roster->students[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

static int compare_students(const void *a, const void *b)
{
    const student_t *student_a = (const student_t *)a;
    const student_t *student_b = (const student_t *)b;
    
    // First sort by grade
    if (student_a->grade != student_b->grade) {
        return student_a->grade - student_b->grade;
    }
    
    // Then sort by name alphabetically
    return strcmp(student_a->name, student_b->name);
}

static void sort_roster(roster_t *roster)
{
    if (roster->count > 1) {
        qsort(roster->students, roster->count, sizeof(student_t), compare_students);
    }
}

bool add_student(roster_t *roster, const char *name, uint8_t grade)
{
    if (roster == NULL || name == NULL || roster->count >= MAX_STUDENTS) {
        return false;
    }
    
    // Check if student already exists
    if (student_exists(roster, name)) {
        return false;
    }
    
    // Add student
    strncpy(roster->students[roster->count].name, name, MAX_NAME_LENGTH - 1);
    roster->students[roster->count].name[MAX_NAME_LENGTH - 1] = '\0';
    roster->students[roster->count].grade = grade;
    roster->count++;
    
    // Sort roster to maintain order
    sort_roster(roster);
    
    return true;
}

roster_t get_grade(roster_t *roster, uint8_t desired_grade)
{
    roster_t result = {0};
    
    if (roster == NULL) {
        return result;
    }
    
    for (size_t i = 0; i < roster->count; i++) {
        if (roster->students[i].grade == desired_grade) {
            if (result.count < MAX_STUDENTS) {
                result.students[result.count] = roster->students[i];
                result.count++;
            }
        }
    }
    
    return result;
}
