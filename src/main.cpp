#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "include/stack.h"

int main ()
{
	struct stack_t stk = {0};

	stackCtor (&stk);

	stackPush (&stk, 10);

	elem_t x = stackPop (&stk);

	stackDtor (&stk);

	return 1;
}