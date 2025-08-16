#include "two_bucket.h"
#include <stdbool.h>
#include <stdlib.h>

// Helper function to calculate GCD
static bucket_liters_t gcd(bucket_liters_t a, bucket_liters_t b) {
    while (b != 0) {
        bucket_liters_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// State representation for BFS
typedef struct {
    bucket_liters_t bucket1;
    bucket_liters_t bucket2;
    int moves;
    bucket_id_t goal_bucket;
    bucket_liters_t other_bucket_liters;
} state_t;

// Queue implementation for BFS
typedef struct queue_node {
    state_t state;
    struct queue_node* next;
} queue_node_t;

typedef struct {
    queue_node_t* front;
    queue_node_t* rear;
} queue_t;

static void queue_init(queue_t* q) {
    q->front = q->rear = NULL;
}

static bool queue_is_empty(queue_t* q) {
    return q->front == NULL;
}

static void queue_enqueue(queue_t* q, state_t state) {
    queue_node_t* node = malloc(sizeof(queue_node_t));
    node->state = state;
    node->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
}

static state_t queue_dequeue(queue_t* q) {
    queue_node_t* temp = q->front;
    state_t state = temp->state;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    return state;
}

static void queue_free(queue_t* q) {
    while (!queue_is_empty(q)) {
        queue_dequeue(q);
    }
}

// Hash table for visited states
#define HASH_SIZE 10007

typedef struct hash_node {
    bucket_liters_t bucket1;
    bucket_liters_t bucket2;
    struct hash_node* next;
} hash_node_t;

static hash_node_t* visited[HASH_SIZE];

static void hash_init() {
    for (int i = 0; i < HASH_SIZE; i++) {
        visited[i] = NULL;
    }
}

static void hash_free() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_node_t* node = visited[i];
        while (node) {
            hash_node_t* temp = node;
            node = node->next;
            free(temp);
        }
        visited[i] = NULL;
    }
}

static int hash_function(bucket_liters_t bucket1, bucket_liters_t bucket2) {
    return (bucket1 * 1000 + bucket2) % HASH_SIZE;
}

static bool is_visited(bucket_liters_t bucket1, bucket_liters_t bucket2) {
    int hash = hash_function(bucket1, bucket2);
    hash_node_t* node = visited[hash];
    
    while (node) {
        if (node->bucket1 == bucket1 && node->bucket2 == bucket2) {
            return true;
        }
        node = node->next;
    }
    return false;
}

static void mark_visited(bucket_liters_t bucket1, bucket_liters_t bucket2) {
    int hash = hash_function(bucket1, bucket2);
    hash_node_t* node = malloc(sizeof(hash_node_t));
    node->bucket1 = bucket1;
    node->bucket2 = bucket2;
    node->next = visited[hash];
    visited[hash] = node;
}

bucket_result_t measure(bucket_liters_t bucket_1_size,
                        bucket_liters_t bucket_2_size,
                        bucket_liters_t goal_volume, bucket_id_t start_bucket) {
    
    bucket_result_t result = {false, 0, BUCKET_ID_1, 0};
    
    // Check if it's possible using GCD
    if (goal_volume > bucket_1_size && goal_volume > bucket_2_size) {
        return result;
    }
    
    if (goal_volume % gcd(bucket_1_size, bucket_2_size) != 0) {
        return result;
    }
    
    // Initialize hash table and queue
    hash_init();
    queue_t q;
    queue_init(&q);
    
    // Starting state
    state_t start_state;
    if (start_bucket == BUCKET_ID_1) {
        start_state.bucket1 = bucket_1_size;
        start_state.bucket2 = 0;
    } else {
        start_state.bucket1 = 0;
        start_state.bucket2 = bucket_2_size;
    }
    start_state.moves = 1;
    start_state.goal_bucket = BUCKET_ID_1;
    start_state.other_bucket_liters = 0;
    
    // Check if we already have the goal
    if (start_state.bucket1 == goal_volume) {
        result.possible = true;
        result.move_count = 1;
        result.goal_bucket = BUCKET_ID_1;
        result.other_bucket_liters = start_state.bucket2;
        queue_free(&q);
        hash_free();
        return result;
    }
    if (start_state.bucket2 == goal_volume) {
        result.possible = true;
        result.move_count = 1;
        result.goal_bucket = BUCKET_ID_2;
        result.other_bucket_liters = start_state.bucket1;
        queue_free(&q);
        hash_free();
        return result;
    }
    
    queue_enqueue(&q, start_state);
    mark_visited(start_state.bucket1, start_state.bucket2);
    
    while (!queue_is_empty(&q)) {
        state_t current = queue_dequeue(&q);
        
        // Generate all possible next states
        state_t next_states[6];
        int num_states = 0;
        
        // Fill bucket 1
        next_states[num_states++] = (state_t){bucket_1_size, current.bucket2, current.moves + 1, BUCKET_ID_1, 0};
        
        // Fill bucket 2
        next_states[num_states++] = (state_t){current.bucket1, bucket_2_size, current.moves + 1, BUCKET_ID_2, 0};
        
        // Empty bucket 1
        next_states[num_states++] = (state_t){0, current.bucket2, current.moves + 1, BUCKET_ID_1, 0};
        
        // Empty bucket 2
        next_states[num_states++] = (state_t){current.bucket1, 0, current.moves + 1, BUCKET_ID_2, 0};
        
        // Pour from bucket 1 to bucket 2
        bucket_liters_t pour_amount = current.bucket1;
        if (pour_amount + current.bucket2 > bucket_2_size) {
            pour_amount = bucket_2_size - current.bucket2;
        }
        next_states[num_states++] = (state_t){current.bucket1 - pour_amount, current.bucket2 + pour_amount, current.moves + 1, BUCKET_ID_1, 0};
        
        // Pour from bucket 2 to bucket 1
        pour_amount = current.bucket2;
        if (pour_amount + current.bucket1 > bucket_1_size) {
            pour_amount = bucket_1_size - current.bucket1;
        }
        next_states[num_states++] = (state_t){current.bucket1 + pour_amount, current.bucket2 - pour_amount, current.moves + 1, BUCKET_ID_2, 0};
        
        for (int i = 0; i < num_states; i++) {
            state_t next = next_states[i];
            
            // Check forbidden state: starting bucket empty and other bucket full
            if (start_bucket == BUCKET_ID_1) {
                if (next.bucket1 == 0 && next.bucket2 == bucket_2_size) {
                    continue;
                }
            } else {
                if (next.bucket2 == 0 && next.bucket1 == bucket_1_size) {
                    continue;
                }
            }
            
            // Skip if already visited
            if (is_visited(next.bucket1, next.bucket2)) {
                continue;
            }
            
            // Check if we found the goal
            if (next.bucket1 == goal_volume) {
                result.possible = true;
                result.move_count = next.moves;
                result.goal_bucket = BUCKET_ID_1;
                result.other_bucket_liters = next.bucket2;
                queue_free(&q);
                hash_free();
                return result;
            }
            if (next.bucket2 == goal_volume) {
                result.possible = true;
                result.move_count = next.moves;
                result.goal_bucket = BUCKET_ID_2;
                result.other_bucket_liters = next.bucket1;
                queue_free(&q);
                hash_free();
                return result;
            }
            
            mark_visited(next.bucket1, next.bucket2);
            queue_enqueue(&q, next);
        }
    }
    
    queue_free(&q);
    hash_free();
    return result;
}
