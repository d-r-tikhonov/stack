#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "include/recalloc.h"
#include "include/stack.h"
#include "include/program_logs.h"

//=====================================================================================================================================

static const elem_t Poison          = (elem_t) 0xFEE1DEAD;
static const size_t StackMultiplier =  2;
static const size_t StackInitValue  = 10;

//=====================================================================================================================================

static int isStackEmpty   (stack_t* const stk);
static int isStackCorrect (stack_t* const stk);

//=====================================================================================================================================

void stackCtor (stack_t* const stk)
{
    assert (stk != nullptr);

    void* data = nullptr;

    int err = 0;
        err = isStackEmpty(stk);
    if (err)
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

    if (stk->capacity == stk->size)
    {
        size_t capacity  = stk->capacity;
               capacity *= StackMultiplier;

        void* data = recalloc (data, capacity, sizeof (elem_t));

        if (data == nullptr)
        {
            //err
        } 

        stk->capacity = capacity;
        stk->data     = (elem_t*) data;
    }

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

    if ((stk->capacity / StackMultiplier) >= stk->size && stk->capacity > StackInitValue)
    {
        size_t capacity = stk->capacity;
               capacity = capacity / StackMultiplier;
        
        void* data = recalloc (data, capacity, sizeof(elem_t));

        if (data == nullptr)
        {
            //err
        }

        stk->data     = (elem_t*) data;
        stk->capacity = capacity;
    }

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

static int isStackEmpty (stack_t* const stk)
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
    if (stk->size < 0)
        return 1;
}

void stackDump (stack_t* const stk)
{
    assert (stk != nullptr);

    debugInfo ("----------------------------------------------\n");
    debugInfo ("----------------------------------------------\n");
    debugInfo ("Status:   %8s \n", isStackCorrect (stk) ? "error" : "ok");
    debugInfo ("Size:     %8ld\n", stk->size);
    debugInfo ("Capacity: %8ld\n", stk->capacity);
    debugInfo ("----------------------------------------------\n");

    for (size_t i = 0; i < stk->size; i++) 
    {
            debugInfo ("0x%.4lX stack[%8ld] = %x\n", sizeof (*stk->data) * i, i, stk->data[i]);
    }
    debugInfo ("capacity: %ld\n", stk->capacity);
    debugInfo ("size:     %ld\n", stk->size);
    debugInfo ("----------------------------------------------\n");
    debugInfo ("----------------------------------------------\n");
}