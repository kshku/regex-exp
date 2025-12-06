#include "range.h"

#include "memory.h"
#include "utils.h"

#include <string.h>

RangeOverlapType get_range_overlap_type(Range first, Range second) {
    if (first.end < second.start || first.start > second.end) return RANGE_OVERLAP_TYPE_NO_OVERLAP;

    if (first.start <= second.start) {
        if (first.end >= second.end) return RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY;
        if (first.end < second.end) return RANGE_OVERLAP_TYPE_ENCLOSES_START;
    }

    if (first.start >= second.start) {
        if (first.end > second.end) return RANGE_OVERLAP_TYPE_ENCLOSES_END;
        if (first.end <= second.end) return RANGE_OVERLAP_TYPE_ENCLOSED;
    }

    return RANGE_OVERLAP_TYPE_NO_OVERLAP; // Should not reach here
}

Range *add_range_to_range_list(Range range, Range *range_list, int *range_list_len) {
    int start = 0, end = 0;
    RangeOverlapType start_type = RANGE_OVERLAP_TYPE_NO_OVERLAP;
    for (start = 0; start < *range_list_len; ++start) {
        start_type = get_range_overlap_type(range, range_list[start]);
        if (start_type != RANGE_OVERLAP_TYPE_NO_OVERLAP) break;
    }
    RangeOverlapType end_type = start_type;

    for (end = start; end < *range_list_len; ++end) {
        RangeOverlapType type = get_range_overlap_type(range, range_list[end]);
        if (type == RANGE_OVERLAP_TYPE_NO_OVERLAP) {
            end--;
            break;
        }
        end_type = type;
    }

    switch (start_type) {
        case RANGE_OVERLAP_TYPE_NO_OVERLAP:
            goto no_overlap;
        case RANGE_OVERLAP_TYPE_ENCLOSES_START:
            range_list[start].start = range.start;
            return range_list;
        case RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY:
            range_list[start].start = range.start;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSES_END:
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSED:
            return range_list;
    }

    switch (end_type) {
        case RANGE_OVERLAP_TYPE_ENCLOSES_START:
            range_list[start].end = range_list[end].end;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY:
            range_list[start].end = range.end;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSES_END:
        case RANGE_OVERLAP_TYPE_NO_OVERLAP:
        case RANGE_OVERLAP_TYPE_ENCLOSED:
            SHOULD_NOT_REACH_HERE;
    }

    int new_size = *range_list_len - (end - start);
    memmove(&range_list[start + 1], &range_list[end + 1], (*range_list_len - end) * sizeof(Range));
    range_list = memory_reallocate(range_list, new_size * sizeof(Range));
    *range_list_len = new_size;

    return range_list;

no_overlap:
    range_list = memory_reallocate(range_list, (*range_list_len + 1) * sizeof(Range));
    int i;
    for (i = 0; i < *range_list_len; ++i)
        if (range_list[i].start > range.start)
            break;

    memmove(&range_list[i + 1], &range_list[i], (*range_list_len - i) * sizeof(Range));
    (*range_list_len)++;
    range_list[i] = range;

    return range_list;
}

Range *remove_range_from_range_list(Range range, Range *range_list, int *range_list_len) {
    int start = 0, end = 0;
    RangeOverlapType start_type = RANGE_OVERLAP_TYPE_NO_OVERLAP;
    for (start = 0; start < *range_list_len; ++start) {
        start_type = get_range_overlap_type(range, range_list[start]);
        if (start_type != RANGE_OVERLAP_TYPE_NO_OVERLAP) break;
    }
    RangeOverlapType end_type = start_type;

    for (end = start; end < *range_list_len; ++end) {
        RangeOverlapType type = get_range_overlap_type(range, range_list[end]);
        if (type == RANGE_OVERLAP_TYPE_NO_OVERLAP) {
            end--;
            break;
        }
        end_type = type;
    }

    int remove_from;
    switch (start_type) {
        case RANGE_OVERLAP_TYPE_NO_OVERLAP:
            return range_list;
        case RANGE_OVERLAP_TYPE_ENCLOSES_START:
            range_list[start].start = range.end;
            return range_list;
        case RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY:
            remove_from = start;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSES_END:
            range_list[start].end = range.start - 1;
            remove_from = start + 1;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSED:
            range_list = memory_reallocate(range_list, (*range_list_len + 1) * sizeof(Range));
            memmove(&range_list[start + 2], &range_list[start + 1], (*range_list_len - (start + 1)) * sizeof(Range));
            range_list[start + 1] = (Range){.start = range.end + 1, .end = range_list[start].end};
            range_list[start].end = range.start - 1;
            (*range_list_len)++;
            return range_list;
    }

    int remove_till;
    switch (end_type) {
        case RANGE_OVERLAP_TYPE_ENCLOSES_START:
            range_list[end].start = range.end + 1;
            remove_till = end;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY:
            remove_till = end + 1;
            break;
        case RANGE_OVERLAP_TYPE_ENCLOSES_END:
        case RANGE_OVERLAP_TYPE_NO_OVERLAP:
        case RANGE_OVERLAP_TYPE_ENCLOSED:
            SHOULD_NOT_REACH_HERE;
    }

    int new_size = *range_list_len - (remove_till - remove_from);
    memmove(&range_list[remove_from], &range_list[remove_till], (*range_list_len - remove_till) * sizeof(Range));
    range_list = memory_reallocate(range_list, new_size);
    *range_list_len = new_size;

    return range_list;
}

