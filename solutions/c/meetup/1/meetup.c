#include "meetup.h"
#include <string.h>

// Helper function to check if a year is a leap year
static int is_leap_year(unsigned int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Helper function to get days in a month
static int days_in_month(unsigned int year, unsigned int month) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    return days[month - 1];
}

// Helper function to get day of week for any date (0=Sunday, 6=Saturday)
// Using Doomsday algorithm or simple calculation
static int get_day_of_week(unsigned int year, unsigned int month, unsigned int day) {
    // Simple formula using known reference point
    // January 1, 1900 was a Monday (1)
    int total_days = 0;
    
    // Count days from 1900 to year
    for (int y = 1900; y < (int)year; y++) {
        total_days += is_leap_year(y) ? 366 : 365;
    }
    
    // Count days from January to month
    for (int m = 1; m < (int)month; m++) {
        total_days += days_in_month(year, m);
    }
    
    // Add days
    total_days += day - 1;
    
    // January 1, 1900 was a Monday (1), so we add 1 and mod 7
    return (total_days + 1) % 7;
}

// Helper function to convert day name to number (0=Sunday, 6=Saturday)
static int day_name_to_number(const char *day_of_week) {
    if (strcmp(day_of_week, "Sunday") == 0) return 0;
    if (strcmp(day_of_week, "Monday") == 0) return 1;
    if (strcmp(day_of_week, "Tuesday") == 0) return 2;
    if (strcmp(day_of_week, "Wednesday") == 0) return 3;
    if (strcmp(day_of_week, "Thursday") == 0) return 4;
    if (strcmp(day_of_week, "Friday") == 0) return 5;
    if (strcmp(day_of_week, "Saturday") == 0) return 6;
    return -1; // Invalid day
}

int meetup_day_of_month(unsigned int year, unsigned int month, const char *week,
                        const char *day_of_week)
{
    int target_day = day_name_to_number(day_of_week);
    if (target_day == -1) return -1; // Invalid day name
    
    int days_this_month = days_in_month(year, month);
    
    if (strcmp(week, "teenth") == 0) {
        // Find the day between 13-19
        for (int day = 13; day <= 19; day++) {
            if (get_day_of_week(year, month, day) == target_day) {
                return day;
            }
        }
    } else if (strcmp(week, "last") == 0) {
        // Find the last occurrence by checking backwards from end of month
        for (int day = days_this_month; day >= 1; day--) {
            if (get_day_of_week(year, month, day) == target_day) {
                return day;
            }
        }
    } else {
        // Handle first, second, third, fourth
        int occurrence = 0;
        if (strcmp(week, "first") == 0) occurrence = 1;
        else if (strcmp(week, "second") == 0) occurrence = 2;
        else if (strcmp(week, "third") == 0) occurrence = 3;
        else if (strcmp(week, "fourth") == 0) occurrence = 4;
        else return -1; // Invalid week
        
        int count = 0;
        for (int day = 1; day <= days_this_month; day++) {
            if (get_day_of_week(year, month, day) == target_day) {
                count++;
                if (count == occurrence) {
                    return day;
                }
            }
        }
    }
    
    return -1; // Not found
}
