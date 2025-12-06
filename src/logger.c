#include "logger.h"

#include "defines.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

static const char *log_level_strings[] = {
    "[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "
};

static const char *colors[] = {
    "1;41", "1;31",  "0;33", "0;32", "0;34", "0;37"
};

#ifdef OS_WINDOWS
static bool enableVTProcessing(DWORD handle_type);
#endif

void logger_log(LogLevel level, const char *restrict msg, ...) {
#ifdef OS_WINDOWS
    // Track whether VT process is enabled for handle
    // 0 -> STD_OUTPUT_HANDLE, 1 -> STD_ERROR_HANDLE
    static bool vt_enabled[2] = {false, false};

    // true = 1 and false = 0
    // Using int just because it is used to access the elements of vt_enabled.
    int error = level < LOG_LEVEL_WARN;  // Fatal or Error is error
    FILE *out_file = error ? stderr : stdout;

    // Try to enable VT processing
    if (!vt_enabled[error])  // VT processing is not enabled, try to turn on
        vt_enabled[error] =
            enableVTProcessing(error ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

    // If VT processing enabled, we can print color
    if (vt_enabled[error]) fprintf(out_file, "\x1b[%sm", colors[level]);
#else
    // Track whether we are writitng to terminal or not
    // 0 -> stdout 1 -> stderr
    static bool is_terminal[2] = {false, false};

    int error = level < LOG_LEVEL_WARN;  // Fatal or Error is error
    FILE *out_file = error ? stderr : stdout;

    if (!is_terminal[error])
        is_terminal[error] = isatty(error ? STDERR_FILENO : STDOUT_FILENO);

    // If terminal, we can print color
    if (is_terminal[error]) fprintf(out_file, "\x1b[%sm", colors[level]);
#endif

    fprintf(out_file, "%s", log_level_strings[level]);

    va_list args;
    va_start(args, msg);
    vfprintf(out_file, msg, args);
    va_end(args);

#ifdef OS_WINDOWS
    // If VT processing enabled, reset the color after printing the message
    if (vt_enabled[error]) fprintf(out_file, "\x1b[0m");
#else
    // If terminal, reset the color after printing the message
    if (is_terminal[error]) fprintf(out_file, "\x1b[0m");
#endif

    fprintf(out_file, "\n");

    // Make sure to flush the message if it is error
    if (error) fflush(NULL);
}

#ifdef OS_WINDOWS
static bool enableVTProcessing(DWORD handle_type) {
    HANDLE handle = GetStdHandle(handle_type);
    if (handle == INVALID_HANDLE_VALUE) return false;

    DWORD modes = 0;
    if (!GetConsoleMode(handle, &modes)) return false;

    modes |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING
           | DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(handle, modes)) return false;

    return true;
}
#endif

