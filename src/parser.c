#include "parser.h"

#include <stdbool.h>

typedef enum RepetitionType {
    REPETITION_TYPE_ONCE,
    REPETITION_TYPE_ZERO_OR_MORE,
    REPETITION_TYPE_ONE_OR_MORE,
    REPETITION_TYPE_ZERO_OR_ONE,
} RepetitionType;

typedef struct Token {
    int input;
    RepetitionType repetition;
} Token;

static int parser_parse_character(Parser *parser);

static RepetitionType parser_parse_repetition(Parser *parser);

static bool parser_get_next_token(Parser *parser, Token *token);

static void parser_add_repetition_once(Parser *parser, int input);

static void parser_add_repetition_zero_or_more(Parser *parser, int input);

static void parser_add_repetition_one_or_more(Parser *parser, int input);

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
        case '\\':
            parser->index++;
        default:
            input = parser->src[parser->index];
            break;
        case '.':
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
    if (!parser->src[parser->index]) return false;

    token->input = parser_parse_character(parser);
    token->repetition = parser_parse_repetition(parser);

    return true;
}

static void parser_add_repetition_once(Parser *parser, int input) {
    State *new = state_create(input);

    *parser->cur = new;
    parser->cur = &new->out;

    parser->total_states++;
}

static void parser_add_repetition_zero_or_more(Parser *parser, int input) {
    State *branch = state_create(BRANCH);
    State *new = state_create(input);

    branch->out1 = new;
    new->out = branch;

    *parser->cur = branch;
    parser->cur = &branch->out;
    
    parser->total_states += 2;
}

static void parser_add_repetition_one_or_more(Parser *parser, int input) {
    State *new = state_create(input);
    State *branch = state_create(BRANCH);

    new->out = branch;
    branch->out1 = new;

    *parser->cur = new;
    parser->cur = &branch->out;

    parser->total_states += 2;
}

static void parser_add_repetition_zero_or_one(Parser *parser, int input) {
    State *branch = state_create(BRANCH);
    State *new = state_create(input);
    State *merge = state_create(EPSILON);

    branch->out = merge;
    branch->out1 = new;
    new->out = merge;

    *parser->cur = branch;
    parser->cur = &merge->out;

    parser->total_states += 3;
}

State *parser_parse(Parser *parser) {
    State *head = state_create(EPSILON);
    parser->total_states = 0;
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

    *parser->cur = state_create(MATCH);
    parser->total_states++;

    return head->out;
}

