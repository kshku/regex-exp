#include "regex.h"

#include "parser.h"
#include "memory.h"
#include "utils.h"

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
    regex->cur_states = (State **)memory_allocate(sizeof(State *) * regex->total_states);
    regex->new_states = (State **)memory_allocate(sizeof(State *) * regex->total_states);

    regex_reset(regex);
}


void regex_destroy(Regex *regex) {
    // Collect and destroy all states
    regex->cur_states_len = 0;
    regex->new_states_len = 0;

    regex_collect_states(regex, regex->start);

    if (regex->new_states_len != regex->total_states) QUIT_WITH_FATAL_MSG("Not all states destroyed");

    for (int i = 0; i < regex->new_states_len; ++i) state_destroy(regex->new_states[i]);

    memory_free(regex->cur_states);
    memory_free(regex->new_states);
}

bool regex_step(Regex *regex, char input) {
    for (int i = 0; i < regex->cur_states_len; ++i) {
        switch (regex->cur_states[i]->c) {
            case MATCH:
                regex->cur_states[i]->out
                    ? regex_add_state_to_new_states(regex, regex->cur_states[i]->out)
                    : regex_add_state_to_new_states(regex, regex->cur_states[i]);
                break;
            default:
                if (input != regex->cur_states[i]->c) break;
                /* fallthrough */
            case ANY_CHAR:
                regex_add_state_to_new_states(regex, regex->cur_states[i]->out);
                break;
            case RANGE:
                if (regex->cur_states[i]->range.start <= input && input <= regex->cur_states[i]->range.end)
                    regex_add_state_to_new_states(regex, regex->cur_states[i]->out);
                break;
        }
    }

    regex_swap_cur_and_new(regex);

    // Check whether the machine is currently in accepting state
    if (regex->match && (regex->match->id < regex->cur_states_len) && (regex->match == regex->cur_states[regex->match->id]))
        return true;
    else return false;
}

void regex_reset(Regex *regex) {
    regex->cur_states_len = 0;
    regex->new_states_len = 0;

    regex_add_state_to_new_states(regex, regex->start);
    regex_swap_cur_and_new(regex);
}

bool regex_pattern_in_line(Regex *regex, const char *line) {
    regex_reset(regex);
    bool matched = false;
    for (int i = 0; line[i]; ++i) matched = regex_step(regex, line[i]);
    return matched;
}

static void regex_add_state_to_new_states(Regex *regex, State *state) {
    switch (state->c) {
        case BRANCH:
            regex_add_state_to_new_states(regex, state->out1);
            /* fallthrough */
        case EPSILON:
            regex_add_state_to_new_states(regex, state->out);
            return;
        case MATCH:
            // NOTE: Thompson's nfa has exactly one accepting state
            regex->match = state;
            break;
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

