#include "state.h"

#include <stdlib.h>

State *state_create(int c) {
    State *state = (State *)malloc(sizeof(State));
    if (!state) return state;

    *state = (State){0};
    state->c = c;

    return state;
}

void state_destroy(State *state) {
    free(state);
}
