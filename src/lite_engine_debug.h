#ifndef BLIB_DEBUG_H
#define BLIB_DEBUG_H

#include <stdio.h>

#define BLIB_DEBUG_STRING "[BLIB] "
#define BLIB_LOG_STREAM stdout
#define BLIB_WARNING_STREAM BLIB_LOG_STREAM
#define BLIB_ERROR_STREAM stderr

#define BLIB_ANSI_COLOR_RED	"\x1b[31m"
#define BLIB_ANSI_COLOR_GREEN	"\x1b[32m"
#define BLIB_ANSI_COLOR_YELLOW	"\x1b[33m"
#define BLIB_ANSI_COLOR_BLUE	"\x1b[34m"
#define BLIB_ANSI_COLOR_MAGENTA	"\x1b[35m"
#define BLIB_ANSI_COLOR_CYAN	"\x1b[36m"
#define BLIB_ANSI_COLOR_RESET	"\x1b[0m"

#define debug_log(...) {\
	fprintf(BLIB_LOG_STREAM, BLIB_ANSI_COLOR_CYAN BLIB_DEBUG_STRING __VA_ARGS__);\
	fprintf(BLIB_LOG_STREAM, "\n" BLIB_ANSI_COLOR_RESET);\
}

#define debug_warn(...) {\
	fprintf(BLIB_WARNING_STREAM, BLIB_ANSI_COLOR_YELLOW BLIB_DEBUG_STRING __VA_ARGS__);\
	fprintf(BLIB_WARNING_STREAM, "\n" BLIB_ANSI_COLOR_RESET);\
}

#define debug_error(...) {\
	fprintf(BLIB_ERROR_STREAM, BLIB_ANSI_COLOR_RED BLIB_DEBUG_STRING __VA_ARGS__);\
	fprintf(BLIB_ERROR_STREAM, "\n" BLIB_ANSI_COLOR_RESET);\
}

#define debug_test()\
	debug_log("this is a test message");\
	debug_warn("this is a test warning");\
	debug_error("this is a test error");

#endif // BLIB_DEBUG_H
