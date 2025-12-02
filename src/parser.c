#include "parser.h"

#include <stdbool.h>

/**
 * @enum RepetitionType
 * @brief Enum to represent the repetition type in the regex.
 */
typedef enum RepetitionType {
    REPETITION_TYPE_ONCE, /**< No repetition */
    REPETITION_TYPE_ZERO_OR_MORE, /**< Repeat zero or more times (*) */
    REPETITION_TYPE_ONE_OR_MORE, /**< Repeat one or more time (+) */
    REPETITION_TYPE_ZERO_OR_ONE, /**< Repeat zero or one time (?) */
} RepetitionType;

/**
 * @struct Token 
 * @brief Structure to represent the token.
 */
typedef struct Token {
    int input; /**< Input character */
    RepetitionType repetition; /**< repetition type */
} Token;

/**
 * @brief Parse and get the next character from source.
 *
 * @param parser Pointer parser state
 *
 * @return The character.
 */
static int parser_parse_character(Parser *parser);

/**
 * @brief Parse and get the repetition of the character got form @ref parser_parse_character.
 *
 * @param parser Pointer to parser state
 *
 * @return The @ref RepetitionType.
 */
static RepetitionType parser_parse_repetition(Parser *parser);

/**
 * @brief Helper function to get the next token.
 *
 * @param parser Pointer to parser state
 * @param token Pointer to the token struct
 *
 * @return false if parsing is complete and no token is generated.
 */
static bool parser_get_next_token(Parser *parser, Token *token);

/**
 * @brief Add nfa fragment for a input with no repetition.
 *
 * @param parser Pointer to parser state
 * @param input The input character
 */
static void parser_add_repetition_once(Parser *parser, int input);

/**
 * @brief Add nfa fragment for a input with zero or more repetition.
 *
 * @param parser Pointer to parser state
 * @param input The input character
 */
static void parser_add_repetition_zero_or_more(Parser *parser, int input);

/**
 * @brief Add nfa fragment for a input with one or more repetition.
 *
 * @param parser Pointer to parser state
 * @param input The input character
 */
static void parser_add_repetition_one_or_more(Parser *parser, int input);

/**
 * @brief Add nfa fragment for a input with zero or one repetition.
 *
 * @param parser Pointer to parser state
 * @param input The input character
 */
static void parser_add_repetition_zero_or_one(Parser *parser, int input);

void parser_create(Parser *parser, const char *src) {
    *parser = (Parser){0};
    parser->src = src;
}

void parser_destroy(Parser *parser) {
    *parser = (Parser){0};
}

void parser_reset(Parser *parser) {
    parser->index = 0;
}

static int parser_parse_character(Parser *parser) {
    int input = 0;
    switch (parser->src[parser->index]) {
        case '\\': /** for escaping the special characters (ex: \*) */
            parser->index++;
        default: /** Anything (if the special characters are in the first, then they are directly taken as the characters */
            input = parser->src[parser->index];
            break;
        case '.': /** The special character which matchs to any character */
            input = ANY_CHAR;
            break;
    }

    parser->index++;

    return input;
}

static RepetitionType parser_parse_repetition(Parser *parser) {
    RepetitionType repetition = REPETITION_TYPE_ONCE;
    switch (parser->src[parser->index]) {
        case '*':
            repetition = REPETITION_TYPE_ZERO_OR_MORE;
            break;
        case '+':
            repetition = REPETITION_TYPE_ONE_OR_MORE;
            break;
        case '?':
            repetition = REPETITION_TYPE_ZERO_OR_ONE;
            break;
        default:
            repetition = REPETITION_TYPE_ONCE;
    }

    if (repetition != REPETITION_TYPE_ONCE)
        parser->index++;

    return repetition;
}

static bool parser_get_next_token(Parser *parser, Token *token) {
    // If parsing is completed, return false
    if (!parser->src[parser->index]) return false;

    token->input = parser_parse_character(parser);
    token->repetition = parser_parse_repetition(parser);

    return true;
}

static void parser_add_repetition_once(Parser *parser, int input) {
    // transition on input character, that's all 
    State *new = state_create(input);

    // Previous fragment's output is to this new state
    *parser->cur = new;
    // out goes to next fragment
    parser->cur = &new->out;

    parser->total_states++;
}

static void parser_add_repetition_zero_or_more(Parser *parser, int input) {
    // Create a branch
    State *branch = state_create(BRANCH);
    State *new = state_create(input);

    // One out goes to the state with the input character
    branch->out1 = new;
    // state with input character goes back to branch
    new->out = branch;

    // Previous fragment's output is to the branch
    *parser->cur = branch;
    // And another out is to next fragment
    parser->cur = &branch->out;
    
    parser->total_states += 2;
}

static void parser_add_repetition_one_or_more(Parser *parser, int input) {
    State *new = state_create(input);
    // Create a branch
    State *branch = state_create(BRANCH);

    // State with input character goes to branch
    new->out = branch;
    // branch's one output goes back to new state
    branch->out1 = new;

    // Previous fragment's output is to new state
    *parser->cur = new;
    // Branch's another output goes to next fragment
    parser->cur = &branch->out;

    parser->total_states += 2;
}

static void parser_add_repetition_zero_or_one(Parser *parser, int input) {
    // Create a branch
    State *branch = state_create(BRANCH);
    State *new = state_create(input);
    // Crate state with epsilon transition for merging outputs from branch
    State *merge = state_create(EPSILON);

    // Branch's output goes to merge and new state
    branch->out = merge;
    branch->out1 = new;

    // New State's output goes to merge
    new->out = merge;

    // Previous fragment's output goes to the branch
    *parser->cur = branch;
    // Merge's output goes to next fragment
    parser->cur = &merge->out;

    parser->total_states += 3;
}

State *parser_parse(Parser *parser) {
    // Create a dummy fragment
    State *head = state_create(EPSILON);
    parser->total_states = 0;
    // Set the dummy fragment's output to next fragment
    parser->cur = &head->out;

    Token token;
    while (parser_get_next_token(parser, &token)) {
        switch (token.repetition) {
            case REPETITION_TYPE_ONCE:
                parser_add_repetition_once(parser, token.input);
                break;
            case REPETITION_TYPE_ZERO_OR_MORE:
                parser_add_repetition_zero_or_more(parser, token.input);
                break;
            case REPETITION_TYPE_ONE_OR_MORE:
                parser_add_repetition_one_or_more(parser, token.input);
                break;
            case REPETITION_TYPE_ZERO_OR_ONE:
                parser_add_repetition_zero_or_one(parser, token.input);
                break;
        }
    }

    // Add a state with match to indicate accepting state
    *parser->cur = state_create(MATCH);
    parser->total_states++;

    // Discard the dummy state
    State *temp = head;
    head = head->out;
    free(temp);

    return head;
}

