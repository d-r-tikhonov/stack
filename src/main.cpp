#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "include/stack.h"

int main ()
{
	struct stack_t stk = {};

	stackCtor (&stk);
	stackDump (&stk);

	for (size_t i = 0; i <= 10; i++)
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