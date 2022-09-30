#ifndef RECALLOC_H
#define RECALLOC_H

void* recalloc (void* ptr, size_t num, size_t size) 
{
    size_t newSize = num * size;
    void* newPtr = realloc (ptr, newSize);

    return newPtr;
}

#endif 