#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "stack.h"



int
main(int argc, char *argv[])
{
	Stack *stack;

    stack = newstack(10);

	exit(EXIT_SUCCESS);
}
