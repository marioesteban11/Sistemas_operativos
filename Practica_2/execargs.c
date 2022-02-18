#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum {

	MAX = 100,
	TOKENS = 10,
};

struct Argumentos {

	char *flags;
	char *final;
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

void
funciones(char *script)
{
	int error;
	char *final_ejecucion[TOKENS];

	// Returns first token
	char *rest = script, *token, *path;

	int iterator = 0;

	while ((token = strtok_r(rest, " ", &rest))) {
		if (iterator == 0) {

			final_ejecucion[iterator] = "my script";
			path = token;
			//fprintf(stderr, "HOLAAAAAAAAAAAAAAAAAAA  %s   %d ejecution %s y el path  %s \n", script, iterator, final_ejecucion[iterator], path);
		} else {

			final_ejecucion[iterator] = token;
			//fprintf(stderr, "ADIOOOOOOOOOOOS  %s   %d  ejecution %s\n", script, iterator, final_ejecucion[iterator]);
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
			wtr =
			    waitpid(child_pid, &wstatus,
				    WCONTINUED | WUNTRACED);
			if (wtr == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
			sleep(numero_sleep);
		}
	}

}

int
main(int argc, char *argv[])
{
	int number_sleep = 0;
	int error = 0;

	if (argc <= 2) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		exit(EXIT_FAILURE);
	}

	number_sleep = atoi(argv[1]);

	// SI el número del sleep es 0 falla
	if (number_sleep <= 0) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		exit(EXIT_FAILURE);
	}
	fork_comandos(argv, argc, number_sleep);

	salida(error);
}
