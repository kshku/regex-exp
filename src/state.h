#pragma once

/**
 * @enum Character
 * @brief Enum to represent special characters.
 */
typedef enum Character {
    ANY_CHAR = 256, /**< Match any character */
    MATCH = 257, /**< Pattern matched (accepting state) */
    BRANCH = 258, /**< Without consuming character go in both outs */
    EPSILON = 259, /**< Go without consuming character */
} Character;

typedef struct State State;

/**
 * @struct State state.h
 * @brief Structure to represent the node in NFA.
 */
struct State {
    int c; /**< The character required for transition. */
    State *out; /**< out edge 1 (used always). */
    State *out1; /**< out edge 2 (used when branching is required). */
    int id; /**< The index of the state node in the set of states nfa can exists. */
};

/**
 * @brief Allocates and returns pointer to State.
 *
 * @param c The character to transition to next state
 *
 * @return Malloced pointer to the state.
 */
State *state_create(int c);

/**
 * @brief Frees the allocated state.
 *
 * @param state Pointer to state
 */
void state_destroy(State *state);

