#include "regex.h"

#include "parser.h"

#include <stdlib.h>

static void regex_add_state_to_new_states(Regex *regex, State *state);

static void regex_swap_cur_and_new(Regex *regex);

static void regex_collect_states(Regex *regex, State *state);

static bool regex_collect_states_helper(Regex *regex, State *state);

bool regex_create(Regex *regex, const char *re) {
    *regex = (Regex){0};
    Parser parser;
    parser_create(&parser, re);

    regex->start = parser_parse(&parser);
    regex->total_states = parser.total_states;

    parser_destroy(&parser);

    regex->cur_states = (State **)malloc(sizeof(State *) * regex->total_states);
    regex->new_states = (State **)malloc(sizeof(State *) * regex->total_states);

    regex_reset(regex);
}


void regex_destroy(Regex *regex) {
    regex->cur_states_len = 0;
    regex->new_states_len = 0;
    regex->matched = false;


    regex_collect_states(regex, regex->start);

    for (int i = 0; i < regex->new_states_len; ++i) state_destroy(regex->new_states[i]);

    free(regex->cur_states);
    free(regex->new_states);
}

bool regex_step(Regex *regex, char input) {
    if (regex->matched) return true;

    for (int i = 0; i < regex->cur_states_len; ++i) {
        switch (regex->cur_states[i]->c) {
            default:
                if (input != regex->cur_states[i]->c) break;
            case ANY_CHAR:
                regex_add_state_to_new_states(regex, regex->cur_states[i]->out);
                break;
        }

        if (regex->matched) return true;
    }

    regex_swap_cur_and_new(regex);

    return regex->matched; // will be false
}

void regex_reset(Regex *regex) {
    regex->cur_states_len = 0;
    regex->new_states_len = 0;

    regex_add_state_to_new_states(regex, regex->start);
    regex_swap_cur_and_new(regex);

    regex->matched = false;
}

static void regex_add_state_to_new_states(Regex *regex, State *state) {
    switch (state->c) {
        case BRANCH:
            regex_add_state_to_new_states(regex, state->out1);
        case EPSILON:
            regex_add_state_to_new_states(regex, state->out);
            return;
        case MATCH:
            regex->matched = true;
            return;
    }

    for (int i = 0; i < regex->new_states_len; ++i) 
        if (regex->new_states[i] == state) return;
    regex->new_states[regex->new_states_len++] = state;
}

static void regex_swap_cur_and_new(Regex *regex) {
    State **temp = regex->new_states;
    regex->new_states = regex->cur_states;
    regex->cur_states = temp;

    regex->cur_states_len = regex->new_states_len;
    regex->new_states_len = 0;

}

static void regex_collect_states(Regex *regex, State *state) {
    if (!regex_collect_states_helper(regex, state)) {
        if (state->out) regex_collect_states(regex, state->out);
        if (state->out1) regex_collect_states(regex, state->out1);
    }
}

static bool regex_collect_states_helper(Regex *regex, State *state) {
    for (int i = 0; i < regex->new_states_len; ++i) 
        if (regex->new_states[i] == state) return true;

    regex->new_states[regex->new_states_len++] = state;

    return false;
}

