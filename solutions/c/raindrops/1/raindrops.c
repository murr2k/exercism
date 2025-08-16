#include "raindrops.h"
#include <stdio.h>
#include <string.h>

void convert(char result[], int drops)
{
    result[0] = '\0';
    int has_sound = 0;
    
    if (drops % 3 == 0) {
        strcat(result, "Pling");
        has_sound = 1;
    }
    
    if (drops % 5 == 0) {
        strcat(result, "Plang");
        has_sound = 1;
    }
    
    if (drops % 7 == 0) {
        strcat(result, "Plong");
        has_sound = 1;
    }
    
    if (!has_sound) {
        sprintf(result, "%d", drops);
    }
}