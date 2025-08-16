#include "zebra_puzzle.h"
#include <stdbool.h>

// Define the attributes for each house
typedef enum { ENGLISH, SPANISH, UKRAINIAN, NORWEGIAN, JAPANESE } nationality_t;
typedef enum { RED, GREEN, IVORY, YELLOW, BLUE } color_t;
typedef enum { DOG, SNAIL, FOX, HORSE, ZEBRA } pet_t;
typedef enum { COFFEE, TEA, MILK, ORANGE_JUICE, WATER } drink_t;
typedef enum { DANCING, PAINTING, READING, FOOTBALL, CHESS } hobby_t;

// House structure
typedef struct {
    nationality_t nationality;
    color_t color;
    pet_t pet;
    drink_t drink;
    hobby_t hobby;
} house_t;

// Check if current assignment satisfies all constraints
static bool is_valid(house_t houses[5]) {
    // 2. The Englishman lives in the red house.
    for (int i = 0; i < 5; i++) {
        if (houses[i].nationality == ENGLISH && houses[i].color != RED) return false;
        if (houses[i].color == RED && houses[i].nationality != ENGLISH) return false;
    }
    
    // 3. The Spaniard owns the dog.
    for (int i = 0; i < 5; i++) {
        if (houses[i].nationality == SPANISH && houses[i].pet != DOG) return false;
        if (houses[i].pet == DOG && houses[i].nationality != SPANISH) return false;
    }
    
    // 4. The person in the green house drinks coffee.
    for (int i = 0; i < 5; i++) {
        if (houses[i].color == GREEN && houses[i].drink != COFFEE) return false;
        if (houses[i].drink == COFFEE && houses[i].color != GREEN) return false;
    }
    
    // 5. The Ukrainian drinks tea.
    for (int i = 0; i < 5; i++) {
        if (houses[i].nationality == UKRAINIAN && houses[i].drink != TEA) return false;
        if (houses[i].drink == TEA && houses[i].nationality != UKRAINIAN) return false;
    }
    
    // 6. The green house is immediately to the right of the ivory house.
    bool found_green_right_of_ivory = false;
    for (int i = 0; i < 4; i++) {
        if (houses[i].color == IVORY && houses[i+1].color == GREEN) {
            found_green_right_of_ivory = true;
            break;
        }
    }
    bool has_green = false, has_ivory = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].color == GREEN) has_green = true;
        if (houses[i].color == IVORY) has_ivory = true;
    }
    if (has_green && has_ivory && !found_green_right_of_ivory) return false;
    
    // 7. The snail owner likes to go dancing.
    for (int i = 0; i < 5; i++) {
        if (houses[i].pet == SNAIL && houses[i].hobby != DANCING) return false;
        if (houses[i].hobby == DANCING && houses[i].pet != SNAIL) return false;
    }
    
    // 8. The person in the yellow house is a painter.
    for (int i = 0; i < 5; i++) {
        if (houses[i].color == YELLOW && houses[i].hobby != PAINTING) return false;
        if (houses[i].hobby == PAINTING && houses[i].color != YELLOW) return false;
    }
    
    // 9. The person in the middle house drinks milk.
    if (houses[2].drink != MILK) return false;
    
    // 10. The Norwegian lives in the first house.
    if (houses[0].nationality != NORWEGIAN) return false;
    
    // 11. The person who enjoys reading lives in the house next to the person with the fox.
    bool reading_next_to_fox = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].hobby == READING) {
            if ((i > 0 && houses[i-1].pet == FOX) || (i < 4 && houses[i+1].pet == FOX)) {
                reading_next_to_fox = true;
                break;
            }
        }
    }
    bool has_reading = false, has_fox = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].hobby == READING) has_reading = true;
        if (houses[i].pet == FOX) has_fox = true;
    }
    if (has_reading && has_fox && !reading_next_to_fox) return false;
    
    // 12. The painter's house is next to the house with the horse.
    bool painter_next_to_horse = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].hobby == PAINTING) {
            if ((i > 0 && houses[i-1].pet == HORSE) || (i < 4 && houses[i+1].pet == HORSE)) {
                painter_next_to_horse = true;
                break;
            }
        }
    }
    bool has_painting = false, has_horse = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].hobby == PAINTING) has_painting = true;
        if (houses[i].pet == HORSE) has_horse = true;
    }
    if (has_painting && has_horse && !painter_next_to_horse) return false;
    
    // 13. The person who plays football drinks orange juice.
    for (int i = 0; i < 5; i++) {
        if (houses[i].hobby == FOOTBALL && houses[i].drink != ORANGE_JUICE) return false;
        if (houses[i].drink == ORANGE_JUICE && houses[i].hobby != FOOTBALL) return false;
    }
    
    // 14. The Japanese person plays chess.
    for (int i = 0; i < 5; i++) {
        if (houses[i].nationality == JAPANESE && houses[i].hobby != CHESS) return false;
        if (houses[i].hobby == CHESS && houses[i].nationality != JAPANESE) return false;
    }
    
    // 15. The Norwegian lives next to the blue house.
    bool norwegian_next_to_blue = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].nationality == NORWEGIAN) {
            if ((i > 0 && houses[i-1].color == BLUE) || (i < 4 && houses[i+1].color == BLUE)) {
                norwegian_next_to_blue = true;
                break;
            }
        }
    }
    bool has_norwegian = false, has_blue = false;
    for (int i = 0; i < 5; i++) {
        if (houses[i].nationality == NORWEGIAN) has_norwegian = true;
        if (houses[i].color == BLUE) has_blue = true;
    }
    if (has_norwegian && has_blue && !norwegian_next_to_blue) return false;
    
    return true;
}

// Generate all permutations and check constraints
static bool solve_recursive(house_t houses[5], bool used_nationality[5], bool used_color[5], 
                          bool used_pet[5], bool used_drink[5], bool used_hobby[5], int house_idx) {
    if (house_idx == 5) {
        return is_valid(houses);
    }
    
    // Try all combinations for current house
    for (int n = 0; n < 5; n++) {
        if (used_nationality[n]) continue;
        for (int c = 0; c < 5; c++) {
            if (used_color[c]) continue;
            for (int p = 0; p < 5; p++) {
                if (used_pet[p]) continue;
                for (int d = 0; d < 5; d++) {
                    if (used_drink[d]) continue;
                    for (int h = 0; h < 5; h++) {
                        if (used_hobby[h]) continue;
                        
                        houses[house_idx].nationality = n;
                        houses[house_idx].color = c;
                        houses[house_idx].pet = p;
                        houses[house_idx].drink = d;
                        houses[house_idx].hobby = h;
                        
                        used_nationality[n] = true;
                        used_color[c] = true;
                        used_pet[p] = true;
                        used_drink[d] = true;
                        used_hobby[h] = true;
                        
                        // Early constraint checking for efficiency
                        bool early_valid = true;
                        
                        // Check constraints that can be validated early
                        if (house_idx == 0 && n != NORWEGIAN) early_valid = false; // Constraint 10
                        if (house_idx == 2 && d != MILK) early_valid = false; // Constraint 9
                        if (n == ENGLISH && c != RED) early_valid = false; // Constraint 2
                        if (c == RED && n != ENGLISH) early_valid = false;
                        if (n == SPANISH && p != DOG) early_valid = false; // Constraint 3
                        if (p == DOG && n != SPANISH) early_valid = false;
                        if (c == GREEN && d != COFFEE) early_valid = false; // Constraint 4
                        if (d == COFFEE && c != GREEN) early_valid = false;
                        if (n == UKRAINIAN && d != TEA) early_valid = false; // Constraint 5
                        if (d == TEA && n != UKRAINIAN) early_valid = false;
                        if (p == SNAIL && h != DANCING) early_valid = false; // Constraint 7
                        if (h == DANCING && p != SNAIL) early_valid = false;
                        if (c == YELLOW && h != PAINTING) early_valid = false; // Constraint 8
                        if (h == PAINTING && c != YELLOW) early_valid = false;
                        if (h == FOOTBALL && d != ORANGE_JUICE) early_valid = false; // Constraint 13
                        if (d == ORANGE_JUICE && h != FOOTBALL) early_valid = false;
                        if (n == JAPANESE && h != CHESS) early_valid = false; // Constraint 14
                        if (h == CHESS && n != JAPANESE) early_valid = false;
                        
                        if (early_valid && solve_recursive(houses, used_nationality, used_color, 
                                                         used_pet, used_drink, used_hobby, house_idx + 1)) {
                            return true;
                        }
                        
                        used_nationality[n] = false;
                        used_color[c] = false;
                        used_pet[p] = false;
                        used_drink[d] = false;
                        used_hobby[h] = false;
                    }
                }
            }
        }
    }
    
    return false;
}

solution_t solve_puzzle(void) {
    house_t houses[5];
    bool used_nationality[5] = {false};
    bool used_color[5] = {false};
    bool used_pet[5] = {false};
    bool used_drink[5] = {false};
    bool used_hobby[5] = {false};
    
    solve_recursive(houses, used_nationality, used_color, used_pet, used_drink, used_hobby, 0);
    
    const char *nationality_names[] = {"English", "Spanish", "Ukrainian", "Norwegian", "Japanese"};
    
    solution_t solution;
    for (int i = 0; i < 5; i++) {
        if (houses[i].drink == WATER) {
            solution.drinks_water = nationality_names[houses[i].nationality];
        }
        if (houses[i].pet == ZEBRA) {
            solution.owns_zebra = nationality_names[houses[i].nationality];
        }
    }
    
    return solution;
}
