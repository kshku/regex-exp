#include "regex.h"

#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Add given state to set of new states.
 *
 * @param regex Pointer to regex state
 * @param state Pointer to state to add
 */
static void regex_add_state_to_new_states(Regex *regex, State *state);

/**
 * @brief Swap the current states set and new states set.
 *
 * @param regex Poniter to regex state
 */
static void regex_swap_cur_and_new(Regex *regex);

/**
 * @brief Collect all the states in the nfa in the new set.
 *
 * @param regex Pointer to the regex state
 * @param state Poniter to the starting state
 */
static void regex_collect_states(Regex *regex, State *state);

/**
 * @brief Helper function to collect all states in the nfa in the new set.
 *
 * @param regex Poniter to regex state
 * @param state State to add to the new states set
 *
 * @return true if the given state was already in the new states set.
 */
static bool regex_collect_states_helper(Regex *regex, State *state);

void regex_create(Regex *regex, const char *re) {
    *regex = (Regex){0};

    // Parse (compile) the regex and generate the nfa.
    Parser parser;
    parser_create(&parser, re);

    regex->start = parser_parse(&parser);
    regex->total_states = parser.total_states;

    parser_destroy(&parser);

    // At max automata might be in all the states nfa.
    regex->cur_states = (State **)malloc(sizeof(State *) * regex->total_states);
    regex->new_states = (State **)malloc(sizeof(State *) * regex->total_states);

    regex_reset(regex);
}


void regex_destroy(Regex *regex) {
    // Collect and destroy all states
    regex->cur_states_len = 0;
    regex->new_states_len = 0;
    regex->matched = false;

    regex_collect_states(regex, regex->start);

    if (regex->new_states_len != regex->total_states) printf("Error: Not all states destroyed\n");

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

    if (state->id < regex->new_states_len && regex->new_states[state->id] == state) return;

    state->id = regex->new_states_len;
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
    if (state->id < regex->new_states_len && regex->new_states[state->id] == state) return true;

    state->id = regex->new_states_len;
    regex->new_states[regex->new_states_len++] = state;
    return false;
}

