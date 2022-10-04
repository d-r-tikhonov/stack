#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <string.h>

uint32_t murmurHash (const void* key, size_t size, uint32_t seed);

#endif