#include "react.h"
#include <stdlib.h>
#include <stdbool.h>

#define MAX_CALLBACKS 10
#define MAX_CELLS 100

typedef enum {
    CELL_INPUT,
    CELL_COMPUTE1,
    CELL_COMPUTE2
} cell_type_t;

struct callback_entry {
    void *context;
    callback callback_fn;
    callback_id id;
    bool active;
};

struct cell {
    cell_type_t type;
    int value;
    int last_notified_value;
    
    // For compute cells
    struct cell *dep1, *dep2;
    compute1 compute_fn1;
    compute2 compute_fn2;
    
    // Callbacks
    struct callback_entry callbacks[MAX_CALLBACKS];
    int callback_count;
    callback_id next_callback_id;
    
    // For dependency tracking
    struct cell *dependents[MAX_CELLS];
    int dependent_count;
};

struct reactor {
    struct cell *cells[MAX_CELLS];
    int cell_count;
};

static void propagate_changes(struct cell *cell);
static void add_dependent(struct cell *dependency, struct cell *dependent);
static void compute_cell_value(struct cell *cell);
static void update_cell_values(struct cell *cell, struct cell **changed_cells, int *changed_count);

struct reactor *create_reactor(void) {
    struct reactor *reactor = malloc(sizeof(struct reactor));
    if (!reactor) return NULL;
    
    reactor->cell_count = 0;
    for (int i = 0; i < MAX_CELLS; i++) {
        reactor->cells[i] = NULL;
    }
    
    return reactor;
}

void destroy_reactor(struct reactor *reactor) {
    if (!reactor) return;
    
    for (int i = 0; i < reactor->cell_count; i++) {
        free(reactor->cells[i]);
    }
    free(reactor);
}

struct cell *create_input_cell(struct reactor *reactor, int initial_value) {
    if (!reactor || reactor->cell_count >= MAX_CELLS) return NULL;
    
    struct cell *cell = malloc(sizeof(struct cell));
    if (!cell) return NULL;
    
    cell->type = CELL_INPUT;
    cell->value = initial_value;
    cell->last_notified_value = initial_value;
    cell->dep1 = cell->dep2 = NULL;
    cell->compute_fn1 = NULL;
    cell->compute_fn2 = NULL;
    cell->callback_count = 0;
    cell->next_callback_id = 1;
    cell->dependent_count = 0;
    
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        cell->callbacks[i].active = false;
    }
    
    reactor->cells[reactor->cell_count++] = cell;
    return cell;
}

struct cell *create_compute1_cell(struct reactor *reactor, struct cell *dependency, compute1 compute_fn) {
    if (!reactor || !dependency || !compute_fn || reactor->cell_count >= MAX_CELLS) return NULL;
    
    struct cell *cell = malloc(sizeof(struct cell));
    if (!cell) return NULL;
    
    cell->type = CELL_COMPUTE1;
    cell->dep1 = dependency;
    cell->dep2 = NULL;
    cell->compute_fn1 = compute_fn;
    cell->compute_fn2 = NULL;
    cell->callback_count = 0;
    cell->next_callback_id = 1;
    cell->dependent_count = 0;
    
    compute_cell_value(cell);
    cell->last_notified_value = cell->value;
    
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        cell->callbacks[i].active = false;
    }
    
    add_dependent(dependency, cell);
    reactor->cells[reactor->cell_count++] = cell;
    return cell;
}

struct cell *create_compute2_cell(struct reactor *reactor, struct cell *dep1, struct cell *dep2, compute2 compute_fn) {
    if (!reactor || !dep1 || !dep2 || !compute_fn || reactor->cell_count >= MAX_CELLS) return NULL;
    
    struct cell *cell = malloc(sizeof(struct cell));
    if (!cell) return NULL;
    
    cell->type = CELL_COMPUTE2;
    cell->dep1 = dep1;
    cell->dep2 = dep2;
    cell->compute_fn1 = NULL;
    cell->compute_fn2 = compute_fn;
    cell->callback_count = 0;
    cell->next_callback_id = 1;
    cell->dependent_count = 0;
    
    compute_cell_value(cell);
    cell->last_notified_value = cell->value;
    
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        cell->callbacks[i].active = false;
    }
    
    add_dependent(dep1, cell);
    add_dependent(dep2, cell);
    reactor->cells[reactor->cell_count++] = cell;
    return cell;
}

int get_cell_value(struct cell *cell) {
    if (!cell) return 0;
    return cell->value;
}

void set_cell_value(struct cell *cell, int new_value) {
    if (!cell || cell->type != CELL_INPUT) return;
    
    if (cell->value != new_value) {
        cell->value = new_value;
        propagate_changes(cell);
    }
}

callback_id add_callback(struct cell *cell, void *context, callback callback_fn) {
    if (!cell || !callback_fn || cell->callback_count >= MAX_CALLBACKS) return 0;
    
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (!cell->callbacks[i].active) {
            cell->callbacks[i].context = context;
            cell->callbacks[i].callback_fn = callback_fn;
            cell->callbacks[i].id = cell->next_callback_id++;
            cell->callbacks[i].active = true;
            cell->callback_count++;
            return cell->callbacks[i].id;
        }
    }
    
    return 0;
}

void remove_callback(struct cell *cell, callback_id id) {
    if (!cell) return;
    
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (cell->callbacks[i].active && cell->callbacks[i].id == id) {
            cell->callbacks[i].active = false;
            cell->callback_count--;
            break;
        }
    }
}

static void add_dependent(struct cell *dependency, struct cell *dependent) {
    if (!dependency || !dependent || dependency->dependent_count >= MAX_CELLS) return;
    
    dependency->dependents[dependency->dependent_count++] = dependent;
}

static void compute_cell_value(struct cell *cell) {
    if (!cell) return;
    
    switch (cell->type) {
        case CELL_COMPUTE1:
            if (cell->dep1 && cell->compute_fn1) {
                cell->value = cell->compute_fn1(cell->dep1->value);
            }
            break;
        case CELL_COMPUTE2:
            if (cell->dep1 && cell->dep2 && cell->compute_fn2) {
                cell->value = cell->compute_fn2(cell->dep1->value, cell->dep2->value);
            }
            break;
        case CELL_INPUT:
            // Input cells don't compute values
            break;
    }
}

static void update_cell_values(struct cell *cell, struct cell **changed_cells, int *changed_count) {
    if (!cell) return;
    
    // Update all dependent cells
    for (int i = 0; i < cell->dependent_count; i++) {
        struct cell *dependent = cell->dependents[i];
        int old_value = dependent->value;
        
        compute_cell_value(dependent);
        
        if (dependent->value != old_value) {
            // Add to changed list if not already there
            bool already_added = false;
            for (int j = 0; j < *changed_count; j++) {
                if (changed_cells[j] == dependent) {
                    already_added = true;
                    break;
                }
            }
            if (!already_added && *changed_count < MAX_CELLS) {
                changed_cells[(*changed_count)++] = dependent;
            }
            
            // Recursively update
            update_cell_values(dependent, changed_cells, changed_count);
        }
    }
}

static void propagate_changes(struct cell *cell) {
    if (!cell) return;
    
    struct cell *changed_cells[MAX_CELLS];
    int changed_count = 0;
    
    // First, update all values
    update_cell_values(cell, changed_cells, &changed_count);
    
    // Then fire callbacks for all changed cells
    for (int i = 0; i < changed_count; i++) {
        struct cell *dependent = changed_cells[i];
        
        if (dependent->value != dependent->last_notified_value) {
            // Fire callbacks
            for (int j = 0; j < MAX_CALLBACKS; j++) {
                if (dependent->callbacks[j].active) {
                    dependent->callbacks[j].callback_fn(dependent->callbacks[j].context, dependent->value);
                }
            }
            dependent->last_notified_value = dependent->value;
        }
    }
}
