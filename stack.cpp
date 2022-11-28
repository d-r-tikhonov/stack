#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include "config.h"
#include "stack.h"
#include "errors.h"
#include "hash.h"
#include "recalloc.h"

//=====================================================================================================================================

static const elem_t Poison          = (elem_t) 0xFEE1DEAD;
static const size_t StackMultiplier =                   2;
static const size_t StackInitValue  =                  10;
static const elem_t nullValue       =                 'u';
static FILE* logFile                =              stdout;

//=====================================================================================================================================

static bool isStackEmpty     (stack_t* const stk);
static int  isStackCorrect   (stack_t* const stk);
static void stackIncrease    (stack_t* const stk);
static void stackDecrease    (stack_t* const stk);
static void dataInit         (stack_t* stk);

static elem_t* recallocStack (stack_t* const stk, const size_t capacity);

#ifdef CANARY_PROTECT
    static canary_t*  leftCanary (void* const data);
    static canary_t* rightCanary (void* const data, const size_t capacity);
#endif

#ifdef HASH_PROTECT
    static hash_t hashStack (stack_t *const stk, uint32_t seed);
#endif

static void nullValueSet (elem_t* data, size_t size);

//=====================================================================================================================================

void stackCtor (stack_t* const stk)
{
    ASSERT (stk != nullptr);

    stk->capacity = StackInitValue;
    stk->size     = 0;

    dataInit (stk);

    #ifdef CANARY_PROTECT
        stk-> leftCanary = Canary;
        stk->rightCanary = Canary;
    #endif

    #ifdef HASH_PROTECT
        stk->hash = hashStack (stk, Seed);
    #endif
}

//=====================================================================================================================================

void stackPush (stack_t* stk, const elem_t item)
{
    ASSERT (stk != nullptr);

    stackIncrease (stk);

    stk->data[stk->size] = item;
    stk->size++;

    #ifdef HASH_PROTECT
        stk->hash = hashStack (stk, Seed);
    #endif
}

//=====================================================================================================================================

elem_t stackPop (stack_t* const stk)
{
    ASSERT (stk != nullptr);

    elem_t item = Poison;

    stackDecrease (stk);

    size_t popValue = stk->size - 1;
    item = stk->data[popValue];
    
    stk->size--;
    stk->data[stk->size] = Poison;

    #ifdef HASH_PROTECT
        stk->hash = hashStack (stk, Seed);
    #endif

    return item;
}

//=====================================================================================================================================

void stackDtor (stack_t* const stk)
{   
    ASSERT (stk != nullptr);

    #ifdef CANARY_PROTECT
        free((char*) stk->data - sizeof(canary_t)); 
    #else
        free (stk->data);
    #endif

    stk->data     = nullptr;
    stk->capacity = 0;
    stk->size     = 0;

    #ifdef HASH_PROTECT
        stk->hash = 0;
    #endif

    #ifdef CANARY_PROTECT
        stk-> leftCanary  = 0;
        stk->rightCanary  = 0;
    #endif
}

//=====================================================================================================================================

static bool isStackEmpty (stack_t* const stk)
{
    ASSERT (stk != nullptr);

    if (!(stk -> data == nullptr))
        return 1;
    if (!(stk -> size == 0))
        return 1;
    if (!(stk -> capacity == 0))
        return 1;

    return 0; 
}

//=====================================================================================================================================

static int isStackCorrect (stack_t* const stk)
{
    ASSERT (stk != nullptr);

    if (stk -> data == nullptr)
        return 1;
    if (stk->size > stk->capacity)
        return 1;
    
    return 0;
}

//=====================================================================================================================================

void stackDump (stack_t* const stk)
{
    ASSERT (stk != nullptr);

    if (stk->data == nullptr) 
    {
        fprintf (logFile, "~~~~~~~~~~~~~~~~~~~~START DUMP~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        fprintf (logFile, " Empty stack: %17p\n", stk);
        fprintf (logFile, " Size:     %10llu\n", stk->size);
        fprintf (logFile, " Capacity: %10llu\n", stk->capacity);
        fprintf (logFile, " Address start: nullptr\n");
        fprintf (logFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    } 

    else 
    {
        fprintf (logFile, "~~~~~~~~~~~~~~~~~ START DUMP ~~~~~~~~~~~~~~~~~~\n");
        fprintf (logFile, " Stack: %17p\n", stk);
        fprintf (logFile, " STATUS: %16s\n", "OK!");
        fprintf (logFile, " Size:     %14llu\n", stk->size);
        fprintf (logFile, " Capacity: %14llu\n", stk->capacity);
        fprintf (logFile, " Address start: %p\n", stk->data);
        fprintf (logFile, " Address   end: %p\n", stk->data + sizeof (elem_t) * stk->capacity);
        fprintf (logFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }

    #ifdef CANARY_PROTECT
        fprintf (logFile, " Left  stack canary = %#0X\n", stk->leftCanary);
        fprintf (logFile, " Right stack canary = %#0X\n", stk->rightCanary);
        fprintf (logFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

        fprintf (logFile, " Left  data canary =  %#0X\n Address: %p\n", *leftCanary (stk->data), leftCanary (stk->data));

        fprintf (logFile, " Right data canary =  %#0X\n Address: %p\n", *rightCanary (stk->data, stk->capacity), 
                rightCanary (stk->data, stk->capacity));

        fprintf (logFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    #endif

    for (size_t i = 0; i < stk->capacity; i++) 
    {
        if (stk->data[i] == Poison) 
        {
            fprintf (logFile, "| stack[%7llu] = %18s |\n", i, "Poison");
        }
        else if (stk->data[i] == nullValue)
        {
            fprintf (logFile, "| stack[%7llu] = %18s |\n", i, "NULL Value");
        }
        else 
        {
            fprintf (logFile, "| stack[%7llu] = %18X |\n", i, stk->data[i]);
        }
    }

    fprintf (logFile, "~~~~~~~~~~~~~~~~~~~ END DUMP ~~~~~~~~~~~~~~~~~~~~~\n");
}

//=====================================================================================================================================

#ifdef CANARY_PROTECT
    static canary_t* leftCanary (void* const data)
    {
        ASSERT (data != nullptr);

        return (canary_t*) ((char*) data - sizeof (canary_t));
    }
#endif

//=====================================================================================================================================

#ifdef CANARY_PROTECT
    static canary_t* rightCanary (void* const data, const size_t capacity)
    {   
        ASSERT (data != nullptr);

        return (canary_t*) ((char*) data + (sizeof (elem_t) * (capacity))); 
    }
#endif

//=====================================================================================================================================

static elem_t* recallocStack (stack_t* const stk, const size_t capacity)
{
    ASSERT (stk != nullptr);

    elem_t* data = stk->data;
    
    #ifdef CANARY_PROTECT
        size_t canaryCapacity = capacity * sizeof (elem_t) + 2 * sizeof (canary_t);
        
        if (data != nullptr)
            data = (elem_t*) leftCanary (stk->data);

        data = (elem_t*) realloc (data, canaryCapacity);

        data = (elem_t*)((canary_t*) data + 1);
    #else
        data = (elem_t*) recalloc (data, capacity, sizeof (elem_t));
    #endif

    if (data == nullptr) 
    {
        fprintf (logFile, "Invalid stack reallocation.\n");
        return nullptr;
    }

    nullValueSet (data + stk->size, capacity - stk->size);

    #ifdef CANARY_PROTECT
        *rightCanary (data, capacity) = Canary;
        * leftCanary (data)           = Canary;
    #endif

    return data;
}

//=====================================================================================================================================

static bool isStackFull (const stack_t* const stk)
{
    ASSERT (stk != nullptr);

    return stk->capacity == stk->size;
}

//=====================================================================================================================================

static bool isStackVast (const stack_t* const stk)
{
    ASSERT (stk != nullptr);

    return stk->capacity / StackMultiplier >= stk->size &&
        stk->capacity > StackInitValue; 
}

//=====================================================================================================================================

static void stackIncrease (stack_t* const stk)
{
    if (isStackFull (stk))
    {
        size_t newCapacity = stk->capacity;
        newCapacity = newCapacity * StackMultiplier;
        
        elem_t* data = recallocStack (stk, newCapacity);

        ASSERT (data != nullptr);

        stk->data     = (elem_t*) data;
        stk->capacity = newCapacity;
    }
}

//=====================================================================================================================================

static void stackDecrease (stack_t* const stk)
{
    if (isStackVast (stk))
    {
        size_t newCapacity = stk->capacity;
            newCapacity = newCapacity / StackMultiplier;
        
        elem_t* data = recallocStack (stk, newCapacity);

        ASSERT (data != nullptr);

        stk->data     = (elem_t*) data;
        stk->capacity = newCapacity;
    }
}

//=====================================================================================================================================

#ifdef HASH_PROTECT
    static hash_t hashStack (stack_t *const stk, uint32_t seed)
    {
        ASSERT (stk != nullptr);

        hash_t  stkHash = murmurHash (stk, sizeof (stack_t), seed);
        hash_t dataHash = murmurHash (stk->data, stk->capacity * sizeof (elem_t), seed);

        return stkHash ^ dataHash;
    }
#endif

//=====================================================================================================================================

static void dataInit (stack_t* stk)
{
    #ifdef CANARY_PROTECT
        size_t canaryCapacity = StackInitValue * sizeof (elem_t) + 2 * sizeof (canary_t);
        elem_t* data = (elem_t*) calloc (canaryCapacity, sizeof (char*));

        data = (elem_t*) ((canary_t*) data + 1);

        nullValueSet (data, stk->capacity);

        * leftCanary (data)                = Canary;
        *rightCanary (data, stk->capacity) = Canary; 

        stk->data = data;
    #endif

    #ifndef CANARY_PROTECT
        elem_t* data = (elem_t*) calloc (StackInitValue, sizeof (elem_t));
        
        stk->data = data;
    #endif
}

//=====================================================================================================================================

static void nullValueSet (elem_t* data, size_t size)
{   
    for (size_t i = 0; i < size; i++) // TODO memset
    {
        data[i] = nullValue;
    }
}

//=====================================================================================================================================