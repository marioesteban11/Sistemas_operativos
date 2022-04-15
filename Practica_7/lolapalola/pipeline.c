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
	// fprintf(stderr, "%s", script);
	// char *path = (char *)malloc((strlen(ruta) + strlen(script) )*sizeof(char*) );  
	while ((token = strtok_r(rest, " ", &rest))) {
		
		//printf("%s ", rest);
		if (iterator == 0) {
			final_ejecucion[iterator] = "my script";

			for (int i = 0; i < NUMERO_RUTAS; i ++){
				snprintf(ruta_especifica, MAX_COMANDO, "%s%s", rutas[0], token);
				fprintf(stderr, "RUTA_BIN:%s\n", ruta_especifica);
				if (access(ruta_especifica, R_OK) == 0){
					break;
				}
			}
			// strcat(ruta_bin, token);
			// strcat(ruta_usr, token);
		} else {

			final_ejecucion[iterator] = token;
		}

		final_ejecucion[iterator + 1] = NULL;
		iterator++;
	}
	
	///printf("\n");
	fprintf(stderr, "RUTA_BIN:%s RUTA USR: \n", ruta_especifica);
	error = execv(ruta_especifica, final_ejecucion);

	//error = execv(ruta_usr, final_ejecucion);
	if (error < 0) {
		fprintf(stderr, "error: %s No such file or directory\n",
			script);
		exit(1);
	}
}

int
main(int argc, char *argv[])
{
	int status;
	int **pipes = (int**)malloc((argc - 2)* sizeof(int *));
	int *childs = (int*)malloc((argc - 1)* sizeof(int));
	
	// Creamos tantos pipes comocomandos -1 tengamos
	for (int i = 0; i < argc - 2; i ++){
		int *fd = (int*)malloc(2*sizeof(int));
		if(pipe(fd) < 0){
			err(EXIT_FAILURE, "cannot make a pipe");
		}
	 	pipes[i] = fd;
	}

	for (int i = 0; i < argc - 1; i++){
    	childs[i] = fork(); 
		switch(childs[i]){
			case -1:
				err(EXIT_FAILURE, "fork failed");
			case 0:
				if(i == 0 ) {
					for (int j = 1; j < argc - 2; j ++){
						close(pipes[j][READ_END]);
						close(pipes[j][WRITE_END]);
					}
					close(pipes[i][READ_END]);
					dup2(pipes[i][WRITE_END], STDOUT_FILENO); 
					close(pipes[i][WRITE_END]);
				}else if (i == argc - 2){
					for (int j = 0; j < i-1; j++){
						close(pipes[j][READ_END]);
						close(pipes[j][WRITE_END]);
					}

					close(pipes[i-1][WRITE_END]);
					dup2(pipes[i-1][READ_END], STDIN_FILENO); 
					close(pipes[i-1][READ_END]);
				}else {
					for (int j = 0; j < argc - 2; j ++){
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
				funciones(argv[i+1]);
		}
	}
	
	wait(&status);
	if (close(pipes[argc-3][READ_END]) == -1){
		err(EXIT_FAILURE,"close failed");
	}
	free(pipes);
	free(childs);
	exit(EXIT_SUCCESS);
}