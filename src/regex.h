#pragma once

#include "state.h"

typedef struct Regex {
    State *start;

    int total_states;

    State **cur_states;
    int cur_states_len;

    State **new_states;
    int new_states_len;

    bool matched;
} Regex;

bool regex_create(Regex *regex, const char *re);

void regex_destroy(Regex *regex);

bool regex_step(Regex *regex, char input); 

void regex_reset(Regex *regex);

// void regex_run(Regex *regex, const char *input_line);

