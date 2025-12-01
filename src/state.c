#include "state.h"

#include <stdlib.h>

State *state_create(int c) {
    State *state = (State *)malloc(sizeof(State));
    if (!state) return state;

    *state = (State){0};
    state->c = c;

    return state;
}

void state_set_out(State *state, State *out) {
    state->out = out;
}

void state_branch_set_outs(State *state, State *out1, State *out2) {
    state->out = out1;
    state->out1 = out2;
}

void state_destroy(State *state) {
    free(state);
}
