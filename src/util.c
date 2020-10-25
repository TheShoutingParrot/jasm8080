#include "jasm8080.h"

void die(const char *filename, const size_t line, const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

        fprintf(stderr, ANSI_COLOR_RED "FATAL ERROR %s:%04ld: " ANSI_COLOR_RESET , filename, line);
        vfprintf(stderr, fmt, vargs);

        fputc('\n', stderr);

        va_end(vargs);

        exit(EXIT_FAILURE);
}

void info(const char *filename, const size_t line, const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

        fprintf(stdout, "INFO %s:%04ld: ", filename, line);
        vfprintf(stdout, fmt, vargs);

        fputc('\n', stdout);

        va_end(vargs);
}
