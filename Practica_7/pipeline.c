#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


enum {
	TOKENS = 20,
	MAX_COMANDO = 200,
};
// void 
// pipelines(int cantidad, char * argumentos)
// {
// 	int fd[2];
// 	char *str ="Luke, I am your father\n";
// 
// 	if(pipe(fd) < 0)
// 		err(EXIT_FAILURE, "cannot make a pipe");
// 
// 	switch(fork()){
// 	case -1:
// 		err(EXIT_FAILURE, "fork failed");
// 	case 0:
// 		close(fd[1]);
// 		dup2(fd[0], 0);
// 		close(fd[0]);
// 		execl("/usr/bin/tr", "tr", "a-z", "A-Z", NULL);
// 		err(EXIT_FAILURE, "exec failed");
// 	default:
// 		close(fd[0]);
// 		if(write(fd[1], str, strlen(str)) != strlen(str))
// 			err(EXIT_FAILURE, "error writting in pipe");
// 		close(fd[1]);
//  	}
// }



void
funciones(char *script)
{
	int error;
	char *final_ejecucion[TOKENS];

	char *rest = script, *token;
	int iterator = 0;
	char ruta[MAX_COMANDO] = "/bin/";
	fprintf(stderr, "%s", script);
	// char *path = (char *)malloc((strlen(ruta) + strlen(script) )*sizeof(char*) );  
	while ((token = strtok_r(rest, " ", &rest))) {
		
		printf("%s ", rest);
		if (iterator == 0) {
			final_ejecucion[iterator] = "my script";
			strcat(ruta, token);
		} else {

			final_ejecucion[iterator] = token;
		}
		
		for (int i = 0; i < iterator; i++){
			fprintf(stderr, "%s  ", final_ejecucion[i]);
		}


		final_ejecucion[iterator + 1] = NULL;
		iterator++;
	}
	
	///printf("\n");
	//printf("path:%s \n", path);
	error = execv(ruta, final_ejecucion);
	if (error < 0) {
		fprintf(stderr, "error: %s No such file or directory\n",
			script);
		exit(EXIT_FAILURE);
	}
}

void escribiendo_comando(int ** fd, char * comando, int numero_pipe, int cantidad)
{
	if (numero_pipe == 0){
		close(fd[numero_pipe][1]);
		dup2(fd[numero_pipe][0], 0);
		close(fd[numero_pipe][0]);
		for (int j = 1; j < cantidad - 1; j++){
			close(fd[j][0]);
			close(fd[j][1]);
		}
		funciones(comando);
		
	}else if (numero_pipe == cantidad - 1) {
		close(fd[numero_pipe][0]);
		dup2(fd[numero_pipe][1], 1);
		close(fd[numero_pipe][1]);

		for (int j = numero_pipe - 1; j <= 0; j--)
		{
			close(fd[j][0]);
			close(fd[j][1]);
		}
	}
}
void 
ejecutar(int cantidad, char ** argumentos, int ** fd)
{
	for (int i = 0; i < cantidad -1; i ++){
		switch(fork()){
			case -1:
				err(EXIT_FAILURE, "fork failed");
			case 0:
				escribiendo_comando(fd, argumentos[i], i, cantidad);
				//execl("/usr/bin/tr", "tr", "a-z", "A-Z", NULL);
				//err(EXIT_FAILURE, "exec failed");
			default:
				printf("a\n");
			}
	}
	

}

void 
array_pipes(int cantidad, char ** argumentos)
{
	int **pipes = (int**)malloc((cantidad - 1)* sizeof(int *));
	int fd[2];

	for (int i = 0; i < cantidad - 1; i ++){
		if(pipe(fd) < 0){
			err(EXIT_FAILURE, "cannot make a pipe");
		}
	 	pipes[i] = fd;
		printf("%d: %d\n", pipes[i][0], pipes[i][1]);
	}

	ejecutar(cantidad, argumentos, pipes);

	free(pipes);
}

int
main(int argc, char *argv[])
{

	if (argc != 4) {
		fprintf(stderr,
			"Introduce el número de argumentos correctos\n");
		exit(1);
	}


	// pipelines(argc, argv);
	array_pipes(argc, argv);
	
	exit(EXIT_SUCCESS);
}