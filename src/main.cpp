#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "include/stack.h"

int main ()
{
	struct stack_t stk = {};

	stackCtor (&stk);

	for (size_t i = 0; i < 23; i++)
	{
		stackPush(&stk, i);
	}
	stackDump (&stk);

	stackPop (&stk);
	stackDump (&stk);


	stackDtor (&stk);

	return 1;
}