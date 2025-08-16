#include "gigasecond.h"
#include <time.h>
#include <stdio.h>

#define GIGASECOND 1000000000

void gigasecond(time_t input, char *output, size_t size)
{
    // Add a gigasecond to the input time
    time_t result = input + GIGASECOND;
    
    // Convert to UTC time structure
    struct tm *time_info = gmtime(&result);
    
    // Format the output string
    strftime(output, size, "%Y-%m-%d %H:%M:%S", time_info);
}