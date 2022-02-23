#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum {

	MENOR_CERO = -1,
	NUMERO_INCORRECTO = -2,
	TOKENS = 20,
};

//Ejecita con execv el proceso que viene de la línea de comandos de la terminal
// con todos los parametros correspondientes
// Ej: /bin/ls /tmp
void
funciones(char *script)
{
	int error;
	char *final_ejecucion[TOKENS];

	char *rest = script, *token, *path;

	int iterator = 0;

	while ((token = strtok_r(rest, " ", &rest))) {
		if (iterator == 0) {

			final_ejecucion[iterator] = "my script";
			path = token;
		} else {

			final_ejecucion[iterator] = token;
		}
		final_ejecucion[iterator + 1] = NULL;
		iterator++;
	}

	error = execv(path, final_ejecucion);
	if (error < 0) {
		fprintf(stderr, "error: %s No such file or directory\n",
			script);
		exit(EXIT_FAILURE);
	}
}

//Creamos un proceso hijo y esperamos a que este muera para crear otro hijo
void
fork_comandos(char **comandos, int elementos, int numero_sleep)
{
	pid_t child_pid, wtr;
	int i;
	int wstatus;

	for (i = 2; i < elementos; i++) {
		child_pid = fork();
		switch (child_pid) {
		case -1:
			err(EXIT_FAILURE, "fork failed!");
		case 0:
			funciones(comandos[i]);
			err(1, "exec failed");
		default:
			wtr = waitpid(child_pid, &wstatus, 0);
			if (wtr == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
			sleep(numero_sleep);
		}
	}
}

//Comprobamos que el parametro que nos pasan por linea de comandos sea un numero
long
get_number_sleep(char *numero)
{
	long number_sleep;
	char *ptr;

	number_sleep = strtol(numero, &ptr, 10);

	if (strcmp(ptr, "\0") != 0) {
		number_sleep = NUMERO_INCORRECTO;
	} else {
		if (number_sleep <= 0) {
			number_sleep = MENOR_CERO;
		}
	}

	return number_sleep;
}

int
main(int argc, char *argv[])
{
	long number_sleep = 0;

	if (argc <= 2) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		exit(EXIT_FAILURE);
	}

	number_sleep = get_number_sleep(argv[1]);

	if (number_sleep < 0) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		exit(EXIT_FAILURE);
	}

	fork_comandos(argv, argc, number_sleep);

	exit(EXIT_SUCCESS);
}
