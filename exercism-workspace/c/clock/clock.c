#include "clock.h"
#include <stdio.h>
#include <string.h>

// Normalize total minutes to a 24-hour format (0-1439 minutes)
static int normalize_minutes(int total_minutes)
{
    total_minutes = total_minutes % (24 * 60);
    if (total_minutes < 0) {
        total_minutes += 24 * 60;
    }
    return total_minutes;
}

clock_t clock_create(int hour, int minute)
{
    clock_t clock;
    int total_minutes = hour * 60 + minute;
    total_minutes = normalize_minutes(total_minutes);
    
    int normalized_hour = total_minutes / 60;
    int normalized_minute = total_minutes % 60;
    
    snprintf(clock.text, MAX_STR_LEN, "%02d:%02d", normalized_hour, normalized_minute);
    return clock;
}

clock_t clock_add(clock_t clock, int minute_add)
{
    // Extract current hour and minute from the clock text
    int hour, minute;
    sscanf(clock.text, "%d:%d", &hour, &minute);
    
    // Create a new clock with the added minutes
    return clock_create(hour, minute + minute_add);
}

clock_t clock_subtract(clock_t clock, int minute_subtract)
{
    return clock_add(clock, -minute_subtract);
}

bool clock_is_equal(clock_t a, clock_t b)
{
    return strcmp(a.text, b.text) == 0;
}
