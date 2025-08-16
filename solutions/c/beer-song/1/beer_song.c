#include "beer_song.h"
#include <stdio.h>
#include <string.h>

static void get_bottle_text(uint8_t bottles, char *buffer, int capitalize)
{
    if (bottles == 0) {
        strcpy(buffer, capitalize ? "No more bottles" : "no more bottles");
    } else if (bottles == 1) {
        strcpy(buffer, "1 bottle");
    } else {
        sprintf(buffer, "%d bottles", bottles);
    }
}

static void get_take_action(uint8_t bottles, char *buffer)
{
    if (bottles == 1) {
        strcpy(buffer, "Take it down and pass it around");
    } else {
        strcpy(buffer, "Take one down and pass it around");
    }
}

static void get_replenish_action(char *buffer)
{
    strcpy(buffer, "Go to the store and buy some more");
}

void recite(uint8_t start_bottles, uint8_t take_down, char **song)
{
    int line_index = 0;
    
    for (uint8_t verse = 0; verse < take_down; verse++) {
        uint8_t current_bottles = start_bottles - verse;
        uint8_t next_bottles;
        
        char current_bottle_text[50];
        char next_bottle_text[50];
        char action_text[50];
        
        // Get current bottle text
        get_bottle_text(current_bottles, current_bottle_text, 1);
        
        // Determine next bottle count and text
        if (current_bottles == 0) {
            next_bottles = 99;
            get_bottle_text(next_bottles, next_bottle_text, 0);
            get_replenish_action(action_text);
        } else {
            next_bottles = current_bottles - 1;
            get_bottle_text(next_bottles, next_bottle_text, 0);
            get_take_action(current_bottles, action_text);
        }
        
        // First line of verse
        if (current_bottles == 0) {
            sprintf(song[line_index], "No more bottles of beer on the wall, no more bottles of beer.");
        } else if (current_bottles == 1) {
            sprintf(song[line_index], "1 bottle of beer on the wall, 1 bottle of beer.");
        } else {
            sprintf(song[line_index], "%d bottles of beer on the wall, %d bottles of beer.", 
                    current_bottles, current_bottles);
        }
        line_index++;
        
        // Second line of verse
        sprintf(song[line_index], "%s, %s of beer on the wall.", action_text, next_bottle_text);
        line_index++;
        
        // Add empty line between verses (except after the last verse)
        if (verse < take_down - 1) {
            strcpy(song[line_index], "");
            line_index++;
        }
    }
}
