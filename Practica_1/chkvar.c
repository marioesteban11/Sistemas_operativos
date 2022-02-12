#include <stdlib.h>
#include <stdio.h>
#include <string.h>
enum {
	NO_EXISTE = 1,
	NO_IGUAL,

};

void
salida(int error)
{
	if (error > 0) {
		printf("\n");
		exit(EXIT_FAILURE);
	} else {
		exit(EXIT_SUCCESS);
	}
}

int
main(int argc, char *argv[])
{
	if (argc <= 2 || (argc - 1) % 2 != 0) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		exit(EXIT_FAILURE);
	}
	int error = 0;
	char *var;

	for (int i = 1; i < argc; i = i + 2) {
		var = getenv(argv[i]);
		//printf("VAR %s, segundo parametro %s\n", var, argv[i+1]);
		if (var == NULL) {
			if (error == 0) {
				fprintf(stderr, "Error: ");
			}
			fprintf(stderr, "No existe la variable %s, ", argv[i]);
			error = NO_EXISTE;
		} else {
			if (strcmp(var, argv[i + 1]) != 0) {
				if (error == 0) {
					fprintf(stderr, "Error: ");
				}
				fprintf(stderr, "%s != %s, ", var,
					argv[i + 1]);
				error = NO_IGUAL;
			}
		}
	}

	salida(error);
}
