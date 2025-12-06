// gcc -g -std=c17 test.c logger.c range.c memory.c
#include <stdio.h>

#include "src/regex.h"
#include "src/memory.h"
#include "src/logger.h"

#include <stdbool.h>

int main(int argc, const char **argv) {
    if (argc != 3) {
        LOG_ERROR("Error with arguments. Requried 2 arguments but %d were given", argc - 1);
        LOG_INFO("Usage: regexer \"<text>\" \"<regex>\"");
        return -1;
    }

    const char *text = argv[1];
    const char *re = argv[2];
    // const char *text = "somebody saw nobody";
    // const char *re = "saw";

    Regex regex;
    regex_create(&regex, re);
    print_memory_usage();

    bool matched = false;
    // for (int i = 0; text[i] && !matched; ++i) {
    //     regex_reset(&regex);
    //     for (int j = i; text[j] && !matched; ++j)
    //         matched = regex_step(&regex, text[j]);
    // }
    // matched = regex_pattern_in_text(&regex, text);
    // for (int i = 0; text[i]; ++i) matched = regex_step(&regex, text[i]);
    matched = regex_pattern_in_line(&regex, text);

    if (matched) LOG_INFO("MATCHED!!!");
    else LOG_INFO("NOT MATCHED!!!");

    regex_destroy(&regex);
    print_memory_usage();
}
