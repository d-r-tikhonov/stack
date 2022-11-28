#include <TXLib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "stack.h"

int main ()
{
	stack_t stk = {};

	stackCtor (&stk);
	stackDump (&stk);

	for (elem_t i = 0; i <= 19; i++)
	{
		stackPush (&stk, i);
		stackDump (&stk);
	}

	stackPop  (&stk);
	stackPop  (&stk);
	stackDump (&stk);

	stackDtor (&stk);

	return 0;
}