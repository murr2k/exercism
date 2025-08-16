#include "two_fer.h"
#include <stdio.h>
#include <string.h>

void two_fer(char *buffer, const char *name)
{
    if (name == NULL || strlen(name) == 0) {
        sprintf(buffer, "One for you, one for me.");
    } else {
        sprintf(buffer, "One for %s, one for me.", name);
    }
}