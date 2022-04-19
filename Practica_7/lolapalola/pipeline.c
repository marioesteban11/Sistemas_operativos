#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

enum {
	TOKENS = 20,
	MAX_COMANDO = 200,
	READ_END = 0,
	WRITE_END = 1,
	NUMERO_RUTAS = 2,
};



void
funciones(char *script)
{
	int error;
	char *final_ejecucion[TOKENS];
	char *rest = script, *token;
	int iterator = 0;
	char ruta_especifica [MAX_COMANDO];
	char rutas[NUMERO_RUTAS][MAX_COMANDO] = {{"/bin/"}, {"/usr/bin/"} };

	while ((token = strtok_r(rest, " ", &rest))) {
		
		//printf("%s ", rest);
		if (iterator == 0) {
			final_ejecucion[iterator] = "my script";

			for (int i = 0; i < NUMERO_RUTAS; i ++){
				snprintf(ruta_especifica, MAX_COMANDO, "%s%s", rutas[i], token);
				//fprintf(stderr, "RUTA_BIN:%s\n", ruta_especifica);
				if (access(ruta_especifica, R_OK) == 0){
					break;
				}
			}
		} else {

			final_ejecucion[iterator] = token;
		}

		final_ejecucion[iterator + 1] = NULL;
		iterator++;
	}
	
	///printf("\n");
	fprintf(stderr, "RUTA_BIN:%s RUTA USR: \n", ruta_especifica);
	error = execv(ruta_especifica, final_ejecucion);

	if (error < 0) {
		fprintf(stderr, "error: %s No such file or directory\n",
			script);
		exit(1);
	}
}


void liberar_pipes(int ** pipes, int numero_pipes){
	for (int i = 0; i < numero_pipes; i++){
		free(pipes[i]);
	}
}
// Creamos tantos pipes comocomandos -1 tengamos
void 
crear_pipes(int **pipes, int numero_pipes){
	for (int i = 0; i < numero_pipes; i ++){
		int *fd = (int*)malloc(2*sizeof(int));
		if(pipe(fd) < 0){
			err(EXIT_FAILURE, "cannot make a pipe");
		}
	 	pipes[i] = fd;
	}

}

void 
pipe_inicial(int ** pipes, int i, int numero_pipes)
{
	for (int j = 1; j < numero_pipes ; j ++){
		close(pipes[j][READ_END]);
		close(pipes[j][WRITE_END]);
	}
	close(pipes[i][READ_END]);
	dup2(pipes[i][WRITE_END], STDOUT_FILENO); 
	close(pipes[i][WRITE_END]);
}

void 
pipe_final(int ** pipes, int i)
{
	for (int j = 0; j < i-1; j++){
		close(pipes[j][READ_END]);
		close(pipes[j][WRITE_END]);
	}

	close(pipes[i-1][WRITE_END]);
	dup2(pipes[i-1][READ_END], STDIN_FILENO); 
	close(pipes[i-1][READ_END]);
}


void
pipes_intermedios(int ** pipes, int i, int numero_pipes)
{
	for (int j = 0; j < numero_pipes ; j ++){
		if (j == i) {
			close(pipes[j][READ_END]);
			dup2(pipes[j][WRITE_END], STDOUT_FILENO); 
			close(pipes[j][WRITE_END]);
		}else if (j == i - 1)
		{
			close(pipes[j][WRITE_END]);
			dup2(pipes[j][READ_END], STDIN_FILENO); 
			close(pipes[j][READ_END]);
		}else {
			close(pipes[j][READ_END]);
			close(pipes[j][WRITE_END]);
		}
	}
}


void
pipelines(int numero_entradas, char ** entradas)
{
	int status;
	int **pipes = (int**)malloc((numero_entradas - 2)* sizeof(int *));
	int *childs = (int*)malloc((numero_entradas - 1)* sizeof(int));
	
	crear_pipes(pipes, numero_entradas-2);

	for (int i = 0; i < numero_entradas - 1; i++){
    	childs[i] = fork(); 
		switch(childs[i]){
			case -1:
				err(EXIT_FAILURE, "fork failed");
			case 0:
				if(i == 0 ) {
					pipe_inicial(pipes, i, numero_entradas-2);
				}else if (i == numero_entradas - 2){
					pipe_final(pipes, i);
				}else {
					pipes_intermedios(pipes, i, numero_entradas-2);
				}
				funciones(entradas[i+1]);
		}
	}
	
	wait(&status);
	if (close(pipes[numero_entradas-3][READ_END]) == -1){
		err(EXIT_FAILURE,"close failed");
	}
	//Liberamos toda la memoria utilizada
	liberar_pipes(pipes, numero_entradas - 2);
	free(pipes);
	free(childs);
}

int
main(int argc, char *argv[])
{
	if (argc != 4){
		fprintf(stderr, "Numero de argumentos incorrecto\n");
		exit(EXIT_FAILURE);
	}
	pipelines(argc, argv);

	exit(EXIT_SUCCESS);
}