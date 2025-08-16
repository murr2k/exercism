#include "dnd_character.h"
#include <stdlib.h>
#include <time.h>

// Helper function to compare integers for qsort
static int compare_int(const void *a, const void *b)
{
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int modifier(int score)
{
    // Calculate modifier: (score - 10) / 2, rounded down
    int diff = score - 10;
    if (diff >= 0) {
        return diff / 2;
    } else {
        // For negative numbers, we need to round down (towards negative infinity)
        return (diff - 1) / 2;
    }
}

int ability(void)
{
    static int seed_initialized = 0;
    if (!seed_initialized) {
        srand(time(NULL));
        seed_initialized = 1;
    }
    
    int dice[4];
    // Roll 4 dice
    for (int i = 0; i < 4; i++) {
        dice[i] = 1 + (rand() % 6);  // Random number from 1 to 6
    }
    
    // Sort the dice to easily discard the lowest
    qsort(dice, 4, sizeof(int), compare_int);
    
    // Sum the highest 3 dice (indices 1, 2, 3 after sorting)
    return dice[1] + dice[2] + dice[3];
}

dnd_character_t make_dnd_character(void)
{
    dnd_character_t character;
    
    character.strength = ability();
    character.dexterity = ability();
    character.constitution = ability();
    character.intelligence = ability();
    character.wisdom = ability();
    character.charisma = ability();
    
    // Hitpoints = 10 + constitution modifier
    character.hitpoints = 10 + modifier(character.constitution);
    
    return character;
}
