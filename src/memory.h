#pragma once

#include <stddef.h>

/**
 * @brief Allocate memory (same as malloc()).
 *
 * @param size Bytes to allocate
 *
 * @return Pointer to allocated memory.
 */
void *memory_allocate(size_t size);

/**
 * @brief Free memory (same as free()).
 *
 * @param ptr Pointer to allocated memory
 */
void memory_free(void *ptr);

/**
 * @brief Reallocate memory (same as realloc()).
 *
 * @param ptr Pointer to the memory to reallocate
 * @param new_size New size of the memory
 */
void *memory_reallocate(void *ptr, size_t new_size);

/**
 * @brief Print the memory usage.
 */
void print_memory_usage(void);

