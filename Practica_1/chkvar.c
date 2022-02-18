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
correct_args(int numero_argumentos)
{
	int salida_error = 0;
	if (numero_argumentos <= 2 || (numero_argumentos - 1) % 2 != 0) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		salida_error = 1;
	}
	return salida_error;
}

int
main(int argc, char *argv[])
{
	
	int error = 0;
	char *var;

	if (correct_args(argc) == 1)
	{
		exit(EXIT_FAILURE);
	}
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
				fprintf(stderr, "%s != %s, ", var, argv[i + 1]);
				error = NO_IGUAL;
			}
		}
	}

	salida(error);
}
