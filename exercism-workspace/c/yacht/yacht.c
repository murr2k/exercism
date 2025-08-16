#include "yacht.h"
#include <stdbool.h>

static int count_occurrences(const dice_t dice, int face) {
    int count = 0;
    for (int i = 0; i < 5; i++) {
        if (dice.faces[i] == face) {
            count++;
        }
    }
    return count;
}

static int sum_all_dice(const dice_t dice) {
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += dice.faces[i];
    }
    return sum;
}

static bool is_yacht(const dice_t dice) {
    int first = dice.faces[0];
    for (int i = 1; i < 5; i++) {
        if (dice.faces[i] != first) {
            return false;
        }
    }
    return true;
}

static bool is_full_house(const dice_t dice) {
    int counts[7] = {0}; // Index 0 unused, 1-6 for die faces
    
    for (int i = 0; i < 5; i++) {
        counts[dice.faces[i]]++;
    }
    
    bool has_three = false, has_two = false;
    for (int i = 1; i <= 6; i++) {
        if (counts[i] == 3) has_three = true;
        if (counts[i] == 2) has_two = true;
        if (counts[i] != 0 && counts[i] != 2 && counts[i] != 3) return false;
    }
    
    return has_three && has_two;
}

static bool has_four_of_a_kind(const dice_t dice) {
    for (int face = 1; face <= 6; face++) {
        if (count_occurrences(dice, face) >= 4) {
            return true;
        }
    }
    return false;
}

static int get_four_of_a_kind_value(const dice_t dice) {
    for (int face = 1; face <= 6; face++) {
        if (count_occurrences(dice, face) >= 4) {
            return face * 4;
        }
    }
    return 0;
}

static bool is_little_straight(const dice_t dice) {
    bool has[7] = {false}; // Index 0 unused
    
    for (int i = 0; i < 5; i++) {
        has[dice.faces[i]] = true;
    }
    
    return has[1] && has[2] && has[3] && has[4] && has[5];
}

static bool is_big_straight(const dice_t dice) {
    bool has[7] = {false}; // Index 0 unused
    
    for (int i = 0; i < 5; i++) {
        has[dice.faces[i]] = true;
    }
    
    return has[2] && has[3] && has[4] && has[5] && has[6];
}

int score(dice_t dice, category_t category) {
    switch (category) {
        case ONES:
            return count_occurrences(dice, 1) * 1;
        case TWOS:
            return count_occurrences(dice, 2) * 2;
        case THREES:
            return count_occurrences(dice, 3) * 3;
        case FOURS:
            return count_occurrences(dice, 4) * 4;
        case FIVES:
            return count_occurrences(dice, 5) * 5;
        case SIXES:
            return count_occurrences(dice, 6) * 6;
        case FULL_HOUSE:
            return is_full_house(dice) ? sum_all_dice(dice) : 0;
        case FOUR_OF_A_KIND:
            return has_four_of_a_kind(dice) ? get_four_of_a_kind_value(dice) : 0;
        case LITTLE_STRAIGHT:
            return is_little_straight(dice) ? 30 : 0;
        case BIG_STRAIGHT:
            return is_big_straight(dice) ? 30 : 0;
        case CHOICE:
            return sum_all_dice(dice);
        case YACHT:
            return is_yacht(dice) ? 50 : 0;
        default:
            return 0;
    }
}
