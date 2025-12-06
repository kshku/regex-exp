#pragma once

/**
 * @struct Range range.h
 * @brief Structure to represent the range
 */
typedef struct Range {
    unsigned short start;
    unsigned short end;
} Range;

/**
 * @enum RnageOverlapType
 * @brief Type of the overlap between two ranges.
 */
typedef enum RangeOverlapType {
    RANGE_OVERLAP_TYPE_NO_OVERLAP,
    RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY,
    RANGE_OVERLAP_TYPE_ENCLOSED,
    RANGE_OVERLAP_TYPE_ENCLOSES_START,
    RANGE_OVERLAP_TYPE_ENCLOSES_END
} RangeOverlapType;

/**
 * @brief Get the @ref RangeOverlapType of given two ranges.
 *
 * @note The output is read as first range RangeOverlapType second range.
 * if both ranges are same returns RANGE_OVERLAP_TYPE_ENCLOSES_COMPLETELY.
 *
 * @param first The first range
 * @param second The second range
 *
 * @return The RangeOverlapType.
 */
RangeOverlapType get_range_overlap_type(Range first, Range second);

/**
 * @brief Add the given range to the given range list (reallocates as required).
 *
 * @param range The range to add
 * @param range_list The range_list array
 * @param range_list_len The lenght of range_list
 *
 * @return Range list array.
 */
Range *add_range_to_range_list(Range range, Range *range_list, int *range_list_len);

/**
 * @brief Remove the given range from the given range list (reallocates as required).
 *
 * @param range The range to remove
 * @param range_list The range_list array
 * @param range_list_len The lenght of range_list
 *
 * @return Range list array.
 */
Range *remove_range_from_range_list(Range range, Range *range_list, int *range_list_len);
