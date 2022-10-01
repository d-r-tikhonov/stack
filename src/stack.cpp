#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

#include "include/recalloc.h"
#include "include/stack.h"

//=====================================================================================================================================

static const elem_t Poison          = (elem_t) 0xFEE1DEAD;
static const size_t StackMultiplier =  2;
static const size_t StackInitValue  = 10;
static FILE* logFile                = fopen ("stack.log", "w");

//=====================================================================================================================================

static int isStackEmpty      (stack_t* const stk);
static int isStackCorrect    (stack_t* const stk);
static canary_t*  leftCanary (stack_t* const stk);
static canary_t* rightCanary (stack_t* const stk, const size_t capacity);
static elem_t* recallocStack (stack_t* const stk, const size_t capacity);
static void stackIncrease    (stack_t* const stk);
static void stackDecrease    (stack_t* const stk);

//=====================================================================================================================================

#ifdef CANARY_PROTECT
    const uint32_t Canary = 0xCAFEBABE;
#endif

//=====================================================================================================================================
void stackCtor (stack_t* const stk)
{
    assert (stk != nullptr);

    void* data = nullptr;

    int err = 0;                 // err = err | INVALID_CANARY  <==> err |= INVALID_CANARY
        err = isStackEmpty(stk); // err & STACK_SIZE_BELOW_ZERO, err & INVALID_CANARY
    if (err) //TODO use macro
    {
        //err
    }

    data = calloc (StackInitValue, sizeof (elem_t));
    if (!data)
    {
        //err
    }

    stk->capacity = StackInitValue;
    stk->data     = (elem_t*) data;
    stk->size     = 0;

    #ifdef CANARY_PROTECT
        stk-> leftCanary = Canary;
        stk->rightCanary = Canary;
    #endif

    err = isStackCorrect (stk);

    if (err)
    {
        //err
    }
}

//=====================================================================================================================================

void stackPush (stack_t* stk, const elem_t item)
{
    assert (stk != nullptr);

    int err = 0;
    err = isStackCorrect(stk);
    if (err)
    {
        //err
    }

    stackIncrease (stk);

    stk->data[stk->size] = item;
    stk->size++;

    err = isStackCorrect (stk);

    if (err)
    {
        //err
    }
}

//=====================================================================================================================================

elem_t stackPop (stack_t* const stk)
{
    assert (stk != nullptr);

    int err = 0;
    err = isStackCorrect(stk);
    if (err)
    {
        //err
    }

    size_t item = Poison;

   stackDecrease (stk);

    size_t popValue = stk->size - 1;
    item = stk->data[popValue];
    
    stk->size--;

    stk->data[stk->size] = Poison;

    return item;
}

//=====================================================================================================================================

void stackDtor (stack_t* const stk)
{   
    assert (stk != nullptr);

    free (stk->data);   
}

//=====================================================================================================================================

static int isStackEmpty (stack_t* const stk) //TODO return bool
{
    assert (stk != nullptr);

    if (!(stk -> data == nullptr))
        return 1;
    if (!(stk -> size == 0))
        return 1;
    if (!(stk -> capacity == 0))
        return 1;

    return 0; 
}

static int isStackCorrect (stack_t* const stk)
{
    assert (stk != nullptr);

    if (stk -> data == nullptr)
        return 1;
    if (stk->size > stk->capacity)
        return 1;
    
    return 0;
}

void stackDump (stack_t* const stk)
{
    assert (stk != nullptr);

    
    fprintf (logFile, "\n----------------------------------------------\n");
    fprintf (logFile, "Status:   %8s \n", isStackCorrect (stk) ? "error" : "ok");
    fprintf (logFile, "Size:     %8d\n", stk->size);
    fprintf (logFile, "Capacity: %8d\n", stk->capacity);
    fprintf (logFile, "----------------------------------------------\n");

    for (size_t i = 0; i < stk->capacity; i++) 
    {
            fprintf (logFile, "0x%.4X stack[%8d] = %x\n", sizeof (*stk->data) * i, i, stk->data[i]);
    }
    fprintf (logFile, "----------------------------------------------\n");
}

//=====================================================================================================================================

static canary_t* leftCanary (stack_t* const stk)
{
    return (canary_t*)((char*) stk->data - sizeof (canary_t));
}

//=====================================================================================================================================

static canary_t* rightCanary (stack_t* const stk, const size_t capacity)
{

}

//=====================================================================================================================================

static elem_t* recallocStack (stack_t* const stk, const size_t capacity)
{
    size_t newCapacity = 0xDEAD;

    elem_t* data = (elem_t*) recalloc (stk->data, newCapacity, sizeof(elem_t));
    if (data == nullptr)
    {
        fprintf (stdout, "Error! In recallocStack (stack_t* const stk, const size_t capacity){}. Invalid allocation!");
        return nullptr;
    }

    memset (data + stk->capacity, 0, newCapacity - stk->capacity * sizeof(elem_t));

    #ifdef CANARY_PROTECT
        * leftCanary (stk)              = Canary;
        *rightCanary (stk, newCapacity) = Canary;
    #endif

    return data;
}

//=====================================================================================================================================

static void stackIncrease (stack_t* const stk)
{
    if ((stk->capacity / StackMultiplier) >= stk->size && stk->capacity > StackInitValue)
    {
        size_t capacity = stk->capacity;
               capacity = capacity / StackMultiplier;
        
        elem_t* data = recallocStack (stk, capacity);

        if (data == nullptr)
        {
            //err
        }

        stk->data     = (elem_t*) data;
        stk->capacity = capacity;
    }
}

//=====================================================================================================================================

static void stackDecrease (stack_t* const stk)
{
    if ((stk->capacity / StackMultiplier) >= stk->size && stk->capacity > StackInitValue)
    {
        size_t capacity = stk->capacity;
               capacity = capacity / StackMultiplier;
        
        elem_t* data = recallocStack (stk, capacity);

        if (data == nullptr)
        {
            //err
        }

        stk->data     = (elem_t*) data;
        stk->capacity = capacity;
    }
}