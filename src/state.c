#include "state.h"

#include <stdlib.h>
#include "memory.h"

State *state_create(int c) {
    State *state = (State *)memory_allocate(sizeof(State));

    *state = (State){0};
    state->c = c;

    return state;
}

void state_destroy(State *state) {
    memory_free(state);
}
