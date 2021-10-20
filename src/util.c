#include "jaz80.h"

void die(const char *filename, const size_t line, const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

        fprintf(stderr, ANSI_COLOR_RED "FATAL ERROR %s:%04zu: " ANSI_COLOR_RESET , filename, line);
        vfprintf(stderr, fmt, vargs);

        fputc('\n', stderr);

        va_end(vargs);

        exit(EXIT_FAILURE);
}

void info(const char *filename, const size_t line, const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

        printf("INFO %s:%04zu: ", filename, line);
        vprintf(fmt, vargs);

        putchar('\n');

        va_end(vargs);
}

bool prompt(const char *promptText, ...) {
        va_list vargs;
	char input;

        va_start(vargs, promptText);

        vprintf(promptText, vargs);

        va_end(vargs);

	printf(" [Y/n] ");

	input = fgetc(stdin);

	
	if(input == '\n')
		return true;

	/* clear stdin */
	char ch;
	while((ch = fgetc(stdin)) != '\n')
		if(ch == EOF)
			break;

	if(tolower(input) == 'y')
		return true;

	return false;
}
