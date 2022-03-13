#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int
combrobar_args(int cantidad_params, char **parametros)
{
	int fallo = 0;

	//printf("%d\n", cantidad_params);
	if (cantidad_params < 4) {
		fallo = -1;
		return fallo;
	}

	if (strcmp(parametros[1], "-f") == 0) {
		fallo = -1;
		return fallo;
	}

	for (int i = 0; i < cantidad_params; i++) {
		if (strcmp(parametros[i], "-f") != 0) {
			fallo = -1;
		} else {
			fallo = 0;
			return fallo;
		}
	}

	return fallo;
}

void
free_resultados(int **resultados, int num_ficheros)
{
	for (int i = 0; i < num_ficheros; i++) {
		free(resultados[i]);
	}
}

void
imprimir_resultado(int **resultados, char **palabras_buscar, char **ficheros,
		   int num_palabras, int num_ficheros)
{
	for (int i = 0; i < num_palabras; i++) {
		printf("\"%s\"	", palabras_buscar[i]);
	}
	printf("\n");

	//Imprimir los resultados en pantalla
	for (int i = 0; i < num_ficheros; i++) {
		for (int j = 0; j < num_palabras; j++) {
			if (resultados[i][j] == 0) {
				printf("x	");
			} else {
				printf("o	");
			}
		}
		printf("%s\n", ficheros[i]);
	}
	printf("\n");
}

int
grep_ficheros(char **ficheros, char **palabras_buscar, int num_ficheros,
	      int num_palabras)
{
	pid_t child_pid, wtr;
	int wstatus;
	int error = 0;

	//int resultados[num_ficheros][num_palabras];
	int **resultados = (int **)malloc(((num_ficheros)) * sizeof(int *));

	for (int i = 0; i < num_ficheros; i++) {
		int *resultados_col =
		    (int *)malloc(((num_palabras)) * sizeof(int *));
		for (int j = 0; j < num_palabras; j++) {
			child_pid = fork();
			switch (child_pid) {
			case -1:
				err(EXIT_FAILURE, "fork failed!");
			case 0:
				error =
				    execl("/usr/bin/fgrep", "my_fgrep", "-q",
					  "-n", "-w", "-s", palabras_buscar[j],
					  ficheros[i], NULL);
				if (error == 256) {
					exit(EXIT_FAILURE);
				}
			default:
				resultados_col[j] = child_pid;
				resultados[i] = resultados_col;
			}
		}
	}

	for (int i = 0; i < num_ficheros; i++) {
		for (int j = 0; j < num_palabras; j++) {
			wtr = waitpid(resultados[i][j], &wstatus, 0);
			if (wtr == -1) {
				perror("wait pid");
				exit(EXIT_FAILURE);
			}
			if (wstatus == 0) {
				resultados[i][j] = wstatus;	// si es status es 0 es que si existe
			} else if (wstatus == 512) {
				fprintf(stderr, "error processing file %s\n",
					ficheros[i]);
				exit(EXIT_FAILURE);
			} else {
				resultados[i][j] = 1;	// si el status es 1 es que no está
			}
		}
	}

	imprimir_resultado(resultados, palabras_buscar, ficheros, num_palabras,
			   num_ficheros);
	free_resultados(resultados, num_ficheros);
	free(resultados);

	return 0;
}

void
free_palabras_concretas(char **palabras_buscar, char **ficheros,
			int num_palabras, int num_ficheros)
{
	for (int i = 0; i < num_palabras; i++) {
		free(palabras_buscar[i]);
	}

	for (int i = 0; i < num_ficheros; i++) {
		free(ficheros[i]);
	}

}

void
separar_argumentos(int cantidad_params, char **parametros)
{
	int encontrar_f = 0;
	char *palabra_fichero = NULL, *palabra = NULL;
	int num_palabras = 0, num_ficheros = 0, num_menos_f = 0;

	for (int i = 0; i < cantidad_params; i++) {
		if (strcmp(parametros[i], "-f") == 0) {
			encontrar_f = i + 1;
			num_menos_f++;
			printf("%d\n", num_menos_f);
			if (num_menos_f > 1){
				printf("usage: grepmatrix word [words ...] -f file [file ...].\n");
				exit(EXIT_FAILURE);
			}
		}
		
	}
	num_palabras = encontrar_f - 2;
	num_ficheros = cantidad_params - encontrar_f;

	if (num_ficheros < 1 || num_palabras < 1){
		printf("usage: grepmatrix word [words ...] -f file [file ...].\n");
		exit(EXIT_FAILURE);
	}

	char **palabras_buscar =
	    (char **)malloc(((num_palabras)) * sizeof(char **));

	char **ficheros = (char **)malloc(((num_ficheros) * sizeof(char **)));

	for (int i = 0; i < num_palabras; i++) {
		palabra =
		    malloc((strlen((parametros[i]) + 1) * sizeof(char *)));

		strcpy(palabra, parametros[i + 1]);
		palabras_buscar[i] = palabra;
	}
	printf("\n");

	for (int i = encontrar_f; i < cantidad_params; i++) {
		palabra_fichero =
		    malloc((strlen((parametros[i]) + 1) * sizeof(char *)));

		strcpy(palabra_fichero, parametros[i]);

		ficheros[i - encontrar_f] = palabra_fichero;	//parametros[i];

	}
	grep_ficheros(ficheros, palabras_buscar, num_ficheros, num_palabras);
	free_palabras_concretas(palabras_buscar, ficheros, num_palabras,
				num_ficheros);

	free(palabras_buscar);
	free(ficheros);
}

int
main(int argc, char *argv[])
{

	if (combrobar_args(argc, argv) != 0) {
		fprintf(stderr,
			"usage: grepmatrix word [words ...] -f file [file ...].\n");
		exit(EXIT_FAILURE);
	}

	separar_argumentos(argc, argv);

	exit(EXIT_SUCCESS);
}
