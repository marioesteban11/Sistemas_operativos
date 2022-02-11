#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	if (argc <= 2) {
		fprintf(stderr, "usage: ch varname varcontent [varname varcontent] ...\n");
		exit(1);
	}
	int error;
	char *var;
	for (int i = 1; i < argc; i=i+2)
	{
		var = getenv(argv[i]);
		//printf("VAR %s, segundo parametro %s\n", var, argv[i+1]);
		if (var == NULL)
		{
			fprintf(stderr, "No existe la variable %s   \n", argv[i]);
			error = 1;
		}else {
			if (strcmp(var, argv[i+1]) != 0)
			{
				fprintf(stderr, "%s != %s   ", var, argv[i+1]);
				error = 1;
			}
		}
	}

	if (error == 1)
	{
		exit(EXIT_FAILURE);
	}else {
		exit(EXIT_SUCCESS);
	}
}
