#pragma once

#include "state.h"

/**
 * @struct regex.h
 * @brief Regex state structure.
 */
typedef struct Regex {
    State *start; /**< Start state of nfa */

    int total_states; /**< Total number of states in nfa */

    State **cur_states; /**< Set of current states the nfa is in */
    int cur_states_len; /**< Lenght of the current states set */

    State **new_states; /**< Set of new states the nfa will be on getting input */
    int new_states_len; /**< Length of the new states set */

    bool matched; /**< The regex matched or not */
} Regex;

/**
 * @brief Create the regex.
 *
 * @param regex Pointer to the regex state
 * @param re The regex string
 */
void regex_create(Regex *regex, const char *re);

/**
 * @brief Destroy the regex.
 *
 * @param regex Pointer to the regex state
 */
void regex_destroy(Regex *regex);

/**
 * @brief Step in nfa by taking the input character.
 *
 * @param regex Pointer to the regex state
 * @param input The input character
 *
 * @return Returns true if nfa is in accepting state (text matched).
 */
bool regex_step(Regex *regex, char input); 

/**
 * @brief Reset the regex state (so that restart the matching).
 * 
 * @param regex Pointer to regex state
 */
void regex_reset(Regex *regex);

// void regex_run(Regex *regex, const char *input_line);

