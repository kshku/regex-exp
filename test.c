#include <stdio.h>

#include "src/regex.h"

int main(void) {
    const char *text = "somebody saw nobody";
    const char *re = "saw";

    Regex regex;
    regex_create(&regex, re);

    bool matched = false;
    for (int i = 0; text[i] && !matched; ++i)
        matched = regex_step(&regex, text[i]);

    if (matched) printf("MATCHED!!!\n");
    else printf("NOT MATCHED!!!\n");

    regex_destroy(&regex);
}
