#include "range.h"
#include "logger.h"

#include <stddef.h>

void print_range_list(Range *range_list, int range_list_len) {
    for (int i = 0; i < range_list_len; ++i)
        LOG_INFO("range_list[%d] = {.start = %d, .end = %d}", i, range_list[i].start, range_list[i].end);
    LOG_INFO("");
}

int main(void) {
    LOG_INFO("Testing add_range_to_range_list: ");
    Range *range_list = NULL;
    int range_list_len = 0;

    range_list = add_range_to_range_list((Range){10, 25}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);
    range_list = add_range_to_range_list((Range){15, 25}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);
    range_list = add_range_to_range_list((Range){5, 5}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);
    range_list = add_range_to_range_list((Range){5, 8}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);
    range_list = add_range_to_range_list((Range){8, 10}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);

    LOG_INFO("Testing remove_range_from_range_list: ");
    range_list = add_range_to_range_list((Range){0, -1}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);

    range_list = remove_range_from_range_list((Range){5, 5}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);

    range_list = remove_range_from_range_list((Range){10, 20}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);

    range_list = remove_range_from_range_list((Range){5, 10}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);

    range_list = remove_range_from_range_list((Range){8, 10}, range_list, &range_list_len);
    print_range_list(range_list, range_list_len);
}
