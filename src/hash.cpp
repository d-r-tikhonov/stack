#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "include/hash.h"

uint32_t murmurHash (const void* key, size_t size, uint32_t seed)
{
    const uint32_t mp = 0x5bd1e995;
    const int sft = 24;

    uint32_t hash = seed ^ (uint32_t) size;

    const unsigned char* data = (const unsigned char*) key;

    while (size >= 4) 
    {
        uint32_t k = *(const uint32_t*) data;

        k *= mp;
        k ^= k >> sft;
        k *= mp;

        hash *= mp;
        hash ^= k;

        data += 4;
        size -= 4;
    }

    uint32_t item = 0;
    switch (size) 
    {
        case 3:
                item = (uint32_t) data[2];
                hash ^= item << 16;
                break;

        case 2:
                item = (uint32_t) data[1];
                hash ^= item << 8u;
                break;

        case 1:
                item = (uint32_t) data[0];
                hash ^= item;
                hash *= mp;
                break;

        default:
                break;
    };

    hash ^= hash >> 13;
    hash *= mp;
    hash ^= hash >> 15;

    return hash;
} 