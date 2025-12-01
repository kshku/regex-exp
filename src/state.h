#pragma once

typedef enum Character {
    ANY_CHAR = 256, // Match any character
    MATCH = 257, // Pattern matched (accepting state)
    BRANCH = 258, // Without consuming character go in both outs
    EPSILON = 259, // Go without consuming character
} Character;

typedef struct State State;

struct State {
    int c;
    State *out;
    State *out1; // branch out
    int id;
};

State *state_create(int c);

void state_set_out(State *state, State *out);

void state_set_branch_outs(State *state, State *out1, State *out2);

void state_destroy(State *state);

