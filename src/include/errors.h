#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>

#ifndef NODEBUG
    #define ASSERT (condition)                                          \
    if (!(condition)){                                                  \
        fprintf (stderr, "Error in %s:\n"                               \
                        "FILE: %s\n"                                    \
                        "LINE: %d\n"                                    \
                        "FUNCTION: %s\n",                               \
               #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);    \
        abort();}
#else
    #define ASSERT (condition) ;
#endif

#endif
