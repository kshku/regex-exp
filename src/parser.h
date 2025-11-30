#pragma once

#include "state.h"

typedef struct Parser {
    const char *src;
    int index;
    State **cur;
    int total_states;
} Parser;

void parser_create(Parser *parser, const char *src);

void parser_destroy(Parser *parser);

void parser_reset(Parser *parser);

State *parser_parse(Parser *parser);
