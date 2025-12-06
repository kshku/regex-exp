#include "parser.h"

#include "utils.h"
#include "range.h"
#include "memory.h"

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

/**
 * @brief Parse and generate nfa for character class/set.
 *
 * @param parser Pointer to the parser state
 */
static void parser_parse_and_generate_character_class(Parser *parser);

/**
 * @brief Function to add the given input character as class character.
 *
 * @param parser Pointer to parser state
 * @param merge Pointer to the merging state
 * @param range The range
 */
static void parser_add_input_range_to_character_class(Parser *parser, State *merge, Range range);

/**
 * @brief Function to parse from wherever index is till alternation or NULL character.
 *
 * @param parser Pointer to parser state.
 */
static State *parser_parse_alternation(Parser *parser);

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

State *parser_parse(Parser *parser) {
    if (!parser->src[parser->index]) QUIT_WITH_FATAL_MSG("Empty regex?"); // Maybe forgot to reset?

    parser->total_states = 0;
    parser->match = state_create(MATCH);
    parser->total_states++;

    parser->head = parser_parse_alternation(parser);

    while (parser->src[parser->index] == '|') {
        parser->index++;
        if (!parser->src[parser->index])
            QUIT_WITH_FATAL_MSG("Expected alternative expression after '|'");
        State *branch = state_create(BRANCH);
        parser->total_states++;
        branch->out = parser_parse_alternation(parser);
        branch->out1 = parser->head;
        parser->head = branch;
    }

    return parser->head;
}

static int parser_parse_character(Parser *parser) {
    int input = 0;
    switch (parser->src[parser->index]) {
        case '\\': /** for escaping the special characters (ex: \*) */
            parser->index++;
            if (!parser->src[parser->index]) QUIT_WITH_FATAL_MSG("Expected another character after '\\'");
            /* fallthrough */
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

static void parser_add_input_range_to_character_class(Parser *parser, State *merge, Range range) {
    State *branch = state_create(BRANCH);
    State *new = state_create(RANGE);
    new->range = range;

    branch->out1 = new;
    new->out = merge;

    *parser->cur = branch;
    parser->cur = &branch->out;

    parser->total_states += 2;
}

static void parser_parse_and_generate_character_class(Parser *parser) {
    if (!parser->src[parser->index]) QUIT_WITH_FATAL_MSG("Expected characters in character class");

    bool negate = parser->src[parser->index] == '^';
    if (negate) parser->index++;

    State *start = state_create(EPSILON);
    State *merge = state_create(EPSILON);
    parser->total_states += 2;

    Range *range_list = NULL;
    int range_list_len = 0;
    if (negate) range_list = add_range_to_range_list((Range){0, LITERAL_CHAR_LAST}, range_list, &range_list_len);

    State **previous_frag_out = parser->cur;
    parser->cur = &start->out;

    if (!parser->src[parser->index]) QUIT_WITH_FATAL_MSG("Expected characters in character class");

    if (parser->src[parser->index] == ']') {
        if (negate) range_list = remove_range_from_range_list((Range){']', ']'}, range_list, &range_list_len);
        else range_list = add_range_to_range_list((Range){']', ']'}, range_list, &range_list_len);
        parser->index++;
    }

    while (parser->src[parser->index] && parser->src[parser->index] != ']') {
        // Parsing time!
        switch(parser->src[parser->index]) {
            case '\\':
                parser->index++;
                if (!parser->src[parser->index]) QUIT_WITH_FATAL_MSG("Expected another character after '\\'");
                /* fallthrough */
            default:
                if (parser->src[parser->index + 1] && parser->src[parser->index + 2]) {
                    if (parser->src[parser->index + 1] == '-' && parser->src[parser->index + 2] != ']') {
                        int end_range_index = parser->index + 2;
                        if (parser->src[end_range_index] == '\\') {
                            if (!parser->src[parser->index + 3]) QUIT_WITH_FATAL_MSG("Expected another character after '\\'");
                            end_range_index++;
                        }

                        if (parser->src[parser->index] >= parser->src[end_range_index])
                            QUIT_WITH_FATAL_MSG("Invalid range '%c-%c' in the character class", parser->src[parser->index], parser->src[end_range_index]);

                        if (negate) range_list = remove_range_from_range_list((Range){parser->src[parser->index], parser->src[end_range_index]}, range_list, &range_list_len);
                        else range_list = add_range_to_range_list((Range){parser->src[parser->index], parser->src[end_range_index]}, range_list, &range_list_len);

                        parser->index = end_range_index;
                        break;
                    }
                }
                if (negate) range_list = remove_range_from_range_list((Range){parser->src[parser->index], parser->src[parser->index]}, range_list, &range_list_len);
                else range_list = add_range_to_range_list((Range){parser->src[parser->index], parser->src[parser->index]}, range_list, &range_list_len);
                break;
        }
        parser->index++;
    }

    // Now time to add all the ranges into nfa!
    for (int i = 0; i < range_list_len; ++i)
        parser_add_input_range_to_character_class(parser, merge, range_list[i]);

    if (parser->src[parser->index] != ']')
        QUIT_WITH_FATAL_MSG("The character class was not closed");

    parser->index++;

    *parser->cur = state_create(DEAD);
    parser->total_states++;

    memory_free(range_list);

    // Handle the repetitions of the character class
    RepetitionType repetition = parser_parse_repetition(parser);
    switch (repetition) {
        case REPETITION_TYPE_ONCE:
            *previous_frag_out = start;
            parser->cur = &merge->out;
            break;
        case REPETITION_TYPE_ZERO_OR_MORE:
            {
                State *branch = state_create(BRANCH);
                parser->total_states++;
                branch->out1 = start;
                merge->out = branch;

                *previous_frag_out = branch;
                parser->cur = &branch->out;
            } break;
        case REPETITION_TYPE_ONE_OR_MORE:
            {
                State *branch = state_create(BRANCH);
                parser->total_states++;

                merge->out = branch;
                branch->out1 = start;

                *previous_frag_out = start;
                parser->cur = &branch->out;
            } break;
        case REPETITION_TYPE_ZERO_OR_ONE:
            {
                state_destroy(*parser->cur);
                parser->total_states--;
                *parser->cur = merge;

                *previous_frag_out = start;
                parser->cur = &merge->out;
            } break;
    }
}

static bool parser_get_next_token(Parser *parser, Token *token) {
    // If parsing is completed, return false
    if (!parser->src[parser->index]) return false;

    // Say this is the end of this part of alternation
    if (parser->src[parser->index] == '|') return false;

    /*
    if (parser->index == 0 && parser->src[parser->index] == '^') {
        token->input = LINE_START;
        token->repetition = REPETITION_TYPE_ONCE;
        parser->index++;
        return true;
    } else*/
    if ((!parser->src[parser->index + 1] || parser->src[parser->index + 1] == '|') && parser->src[parser->index] == '$') {
        token->input = LINE_END;
        token->repetition = REPETITION_TYPE_ONCE;
        parser->index++;
        return true;
    }

    if (parser->src[parser->index] == '[') {
        parser->index++;
        parser_parse_and_generate_character_class(parser);
        if (!parser->src[parser->index]) return false;
    }

    if (parser->src[parser->index] == '(') {
    }

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

static State *parser_parse_alternation(Parser *parser) {
    // Create a dummy fragment
    State *head = state_create(EPSILON);
    // Set the dummy fragment's output to next fragment
    parser->cur = &head->out;

    if (parser->src[parser->index] == '|') QUIT_WITH_FATAL_MSG("Expected alternative expression before '|'");

    if (parser->src[parser->index] != '^') {
        // Create a infinity loop matching any character in the beginning so that
        // nfa does not die when first character doesn't match
        State *branch = state_create(BRANCH);
        State *any_char = state_create(ANY_CHAR);
        parser->total_states += 2;

        // branch's one out goes to any_char
        branch->out1 = any_char;
        // any_char's output goes back to branch
        any_char->out = branch;

        // Previous fragment's output goes to branch
        *parser->cur = branch;
        // branch's out goes to next fragment
        parser->cur = &branch->out;
    } else {
        parser->index++;
    }

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
    *parser->cur = parser->match;
    parser->cur = NULL;


    // Discard the dummy state
    State *temp = head;
    head = head->out;
    state_destroy(temp);

    return head;
}

