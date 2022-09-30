#ifndef PROGRAM_LOGS_H
#define PROGRAM_LOGS_H

#include <stdio.h>

#ifdef NODEBUG
    #define debugInfo(msg, ...) (void(0))
#else
    #define debugInfo(msg, ...)                                         \
        fprintf (stderr, msg, ##__VA_ARGS__);                           \
    #endif

#ifdef NODEBUG
    #define log(msg, ...) (void(0))
#else
    #define log(msg, ...)                                               \
        fprintf (stderr, "[%s] in %s at %s(%d): " msg,                  \
                __TIME__, __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
#endif

