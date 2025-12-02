#pragma once

#include "state.h"

/**
 * @struct parser.h
 * @brief Parser state structure.
 */
typedef struct Parser {
    const char *src; /**< Pointer to src (the regex) */
    int index; /**< The index in src parser is at currently */
    State **cur; /**< Internal pointer used by parser to generate the NFA */
    int total_states; /**< Total number of states allocated */
} Parser;

/**
 * @brief Create the parser.
 *
 * @param parser Pointer to parser state
 * @param src The regex to compile
 */
void parser_create(Parser *parser, const char *src);

/**
 * @brief Destroy the parser.
 *
 * @param parser Pointer to parser state
 */
void parser_destroy(Parser *parser);

/**
 * @brief Reset the parser (so that can generate nfa again).
 *
 * @param parser Pointer to parser state.
 */
void parser_reset(Parser *parser);

/**
 * @brief Parse and generate NFA.
 *
 * @param parser Pointer to the parser state
 *
 * @return Pointer to starting state of NFA.
 */
State *parser_parse(Parser *parser);

