#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>

#define ASSERT (condition)                                              \
    if (!(condition))                                                   \
        fprintf (stdout, "Error: " #condition, "in %d in %s in %s", 
            __LINE__, __FILE__, __PRETTY_FUNCTION__)
#endif

