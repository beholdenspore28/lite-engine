#ifndef LITE_ENGINE_LOG_H
#define LITE_ENGINE_LOG_H

#include <stdio.h>

#define ANSI_COLOR_RED	   "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE	   "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN	   "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define debug_log(...)                                                         \
  {                                                                            \
    printf(ANSI_COLOR_BLUE "[LITE_ENGINE_LOG]\t" __VA_ARGS__);                 \
    printf("\n" ANSI_COLOR_RESET);                                             \
  }

#define debug_warn(...)                                                        \
  {                                                                            \
    printf(ANSI_COLOR_YELLOW "[LITE_ENGINE_WARNING]\t" __VA_ARGS__);           \
    printf("\n" ANSI_COLOR_RESET);                                             \
  }

#define debug_error(...)                                                       \
  {                                                                            \
    printf(ANSI_COLOR_RED "[LITE_ENGINE_ERROR]\t" __VA_ARGS__);                \
    printf("\n" ANSI_COLOR_RESET);                                             \
  }

#define debug_test()                                                           \
  debug_log("this is a test message");                                         \
  debug_warn("this is a test warning");                                        \
  debug_error("this is a test error");

#endif
