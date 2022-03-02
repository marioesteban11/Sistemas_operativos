#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int
combrobar_args(int cantidad_params, char ** parametros)
{
	int fallo = 0;
	//printf("%d\n", cantidad_params);
	if (cantidad_params < 4) {
		fallo = -1;
		return fallo;
	}

	if (strcmp(parametros[1], "-f") == 0){
		fallo = -1;
		return fallo;
	}

	for (int i = 0; i<cantidad_params; i++){
		if (strcmp(parametros[i], "-f") != 0){
			fallo = -1;
		}else {
			fallo = 0;
			return fallo;
		}
	}

	return fallo;
}



//fprep -q ( no escribe nada en la entrada estandar)

//fgrep -w busca una palabra concreta
int 
grep_fork(char *fichero,char ** palabras_buscar, int num_palabras, int *bools_columnas)
{
	pid_t child_pid, wtr;
	int i = 0;
	int wstatus, error = 0;
	//int * bools = malloc(( num_palabras )*sizeof(int)  );
	//int bools[&num_bools];

	for (i = 0; i < num_palabras; i++) {
		//palabra = malloc((strlen((parametros[i]) +1) * sizeof(char*))  );
		child_pid = fork();
		
		switch (child_pid) {
		case -1:
			err(EXIT_FAILURE, "fork failed!");
		case 0:
			//printf("SOY EL SEGUNDO HIJO DE %s CON LA PALABRA A BUSCAR %s\n", fichero, palabras_buscar[i]);
			 //Execv con los parametros de -q, -w, "palabra a buscar", fichero donde buscar
			error = execl("/usr/bin/fgrep", "my_fgrep","-q","-n", "-w", "-s", palabras_buscar[i],fichero, NULL );
			//fprintf(stderr, "ERROOOOOOR %d\n\n", error);
			if (error == 2) {
				err(1, "exec failed");
			}else if(error == -1){ //Falla el execv no el fgrep
				err(1, "exec failed");
			}else{
				exit(EXIT_FAILURE);
			}
		default:
			wtr = waitpid(child_pid, &wstatus, 0);
			if (wtr == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
			if(wstatus == 512) {
				fprintf(stderr, "error processing file %s\n", fichero);
				return -1;
				//exit(EXIT_FAILURE);
			}else if (wstatus != 0) {
				bools_columnas[i] = 1;
			}else {
				bools_columnas[i] = 0;
			}
		}
	}
	
	for (int j = 0; j < num_palabras; j++){
		if (bools_columnas[j] == 0){
			printf("x	");
		}else{
			printf("o	");
		}
	}

	printf("%s\n",fichero);

	return 0;
}

void 
free_columnas(int **bools_filas,int num_ficheros,int num_palabras)
{
	for (int i = 0; i < num_ficheros; i++){
		//for (int j = 0; j < num_palabras; j++){
			free(bools_filas[i]);
		//}
		
	}
}
int 
leer_ficheros(char **ficheros, char** palabras_buscar, int num_ficheros, int num_palabras)
{

	struct stat buffer;

	for (int i = 0; i < num_ficheros; i++){
		int exist = stat (ficheros[i], &buffer);
		if (exist != 0){
			fprintf(stderr, "error processing file %s\n", ficheros[i]);
			return -1;
		}
	}


	for (int i = 0; i < num_palabras; i++){
		printf("\"%s\"	", palabras_buscar[i]);
	}
	printf("\n");


	pid_t child_pid, wtr;
	int i;
	int wstatus;
	int error = 0;
	int *bools_columnas;
	// Probar a cambiar a char, es decir, cambiar los 1 y ceros por x y o en la función de arriba
	int ** bools_filas = malloc( num_ficheros *sizeof(int*)  ); // Matriz donde irán los 1 y los ceros para despues imprimirlos.


	for (i = 0; i < num_ficheros; i++) {
		bools_columnas = malloc(( num_palabras )*sizeof(int)  );
		child_pid = fork();
		//printf("HOLA MUY BUENAS SOY %s con PID %d con estas palabras %d y estos ficheros  %d\n", ficheros[i], child_pid, num_palabras, num_ficheros);
		switch (child_pid) {
		case -1:
			err(EXIT_FAILURE, "fork failed!");
		case 0:
			error = grep_fork(ficheros[i], palabras_buscar, num_palabras, bools_columnas);
			//printf("SOY EL PRIMER HIJO DE %s en la iteracion %d\n", ficheros[i], i);
			bools_filas[i] = bools_columnas;
			//fprintf(stderr, "ERROOOOOOR %d\n", error);
			if (error == -1){
				exit(EXIT_FAILURE);
			}else{
				exit(EXIT_SUCCESS);
			}	
		default:
			wtr = waitpid(child_pid, &wstatus, 0);
			if (wtr == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
		}
	}

	free_columnas(bools_filas, num_ficheros, num_palabras);
	free(bools_filas);
	printf("\n");
	return 0;
}


void 
free_palabras_concretas(char **palabras_buscar, char** ficheros, int num_palabras, int num_ficheros)
{
	for (int i = 0; i < num_palabras; i++)
	{
		free(palabras_buscar[i]);
	}

	for (int i = 0; i < num_ficheros; i++)
	{
		free(ficheros[i]);
	}

}

void 
separar_argumentos(int cantidad_params, char ** parametros)
{
	int encontrar_f = 0;
	char *palabra_fichero = NULL, *palabra = NULL;
	int num_palabras = 0, num_ficheros = 0;
	for (int i = 0; i < cantidad_params; i++){
		if (strcmp(parametros[i], "-f") == 0){
			encontrar_f = i+1;
		}
	}
	num_palabras = encontrar_f - 2;
	num_ficheros = cantidad_params - encontrar_f;

	char **palabras_buscar = (char **)malloc((( num_palabras) )*sizeof(char**)  );

	char **ficheros = (char **)malloc(( (num_ficheros) *sizeof(char**)  ));


	for (int i = 0; i < num_palabras; i++)
	{
		palabra = malloc((strlen((parametros[i]) +1) * sizeof(char*))  );

		strcpy(palabra, parametros[i+1]);
		palabras_buscar[i] = palabra; //parametros[i+1];
	}
	//free(palabra);
	printf("\n");

	for (int i = encontrar_f; i < cantidad_params ; i++)
	{
		palabra_fichero = malloc((strlen((parametros[i]) +1) *sizeof(char*))  );
		
		strcpy(palabra_fichero, parametros[i]);
		
		ficheros[i-encontrar_f] = palabra_fichero; //parametros[i];
		
	}
	leer_ficheros(ficheros, palabras_buscar, num_ficheros, num_palabras);
	free_palabras_concretas(palabras_buscar, ficheros, num_palabras, num_ficheros);

	free(palabras_buscar);
	free(ficheros);
}


int
main(int argc, char *argv[])
{
	
	if (combrobar_args(argc, argv) != 0)
	{
		fprintf(stderr,
			"usage: grepmatrix word [words ...] -f file [file ...].\n");
		exit(EXIT_FAILURE);
	}

	separar_argumentos(argc, argv);

	exit(EXIT_SUCCESS);
}
