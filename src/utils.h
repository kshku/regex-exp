#pragma once

#include "logger.h"
#include <stdlib.h>

/**
 * @brief Quit after printing a fatal message.
 *
 * @param msg The format message
 * @param ... Arguments to the format message
 */
#define QUIT_WITH_FATAL_MSG(msg, ...) do {\
    LOG_FATAL(msg, ##__VA_ARGS__);      \
    exit(EXIT_FAILURE);               \
    } while (0)

/**
 * @brief Mark the execution should not reach here.
 */
#define SHOULD_NOT_REACH_HERE QUIT_WITH_FATAL_MSG("The execution should have not reached here. This bug should be reported details for reproducing the bug!");
