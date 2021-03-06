#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

//access para ver si la ruta es accesible para nosotros

enum {
	SIZE = 8 * 1024,
	MAX_STDIN = 100,
	TOKENS = 20,
	MAX_COMANDO = 200,
	NUMERO_RUTAS = 2,

	READ_END = 0,
	WRITE_END = 1,
};

void 
liberar_argumentos(char ** argumentos, int numero)
{
	for (int i = 0; i < numero; i++){
		free(argumentos[i]);
	}
}


void liberar_pipes(int ** pipes, int numero_pipes){
	for (int i = 0; i < numero_pipes; i++){
		free(pipes[i]);
	}
}

int 
get_num_tokens(char *path, char *separacion)
{
    char * rest_1, *token;
    int longitud = strlen(path), num_tokens = 0;
    char copia_1[longitud];
    snprintf(copia_1, longitud+1, "%s", path);
    rest_1 = copia_1;

    while ((token = strtok_r(rest_1, separacion, &rest_1))) {
		num_tokens++;
	}
    return num_tokens;
}



void
crear_string(char *path, char **lista_path, int num_pipes, char *separacion)
{
    int longitud = strlen(path), iterator = 0;
    char copia_1[longitud];
    char *rest_1, *token;

    snprintf(copia_1, longitud+1, "%s", path);
    rest_1 = copia_1;
    while ((token = strtok_r(rest_1, separacion, &rest_1))) {
        char *ayuda = (char*)malloc(strlen(token)*sizeof(char));
        snprintf(ayuda, strlen(token)+1, "%s", token);
        lista_path[iterator] = ayuda;
        iterator++;
	}
}


void set_variables_entorno(char * expresion){

    char* token;
    char* rest = expresion;
	char * enviroment;
 
	token = strtok_r(rest, "=", &enviroment);

	setenv(token, enviroment, 1);

	//printf("VARIABLE DE ENTORNO: %s valor de la variable de entornos %s \n",token, getenv(token));
	//printf("CHAR PPAL: %s, char secundario %s y rest %s y environment %s\n", expresion, token, rest, enviroment );
}

void
funciones(char *script)
{
	//int error;
	char *final_ejecucion[TOKENS];

	char *rest = script, *token;
	int iterator = 0;
	char ruta_especifica [MAX_COMANDO];
	char *path, separar_path[2] = ":";
	path = getenv("PATH");

	int num_paths = 0;

	num_paths = get_num_tokens(path, separar_path);
    //printf("%s\n", path);
    char **lista_path = (char**)malloc(num_paths*sizeof(char*));

	crear_string(path, lista_path, num_paths, separar_path);
	while ((token = strtok_r(rest, " ", &rest))) {
		
		//printf("%s ", rest);
		if (iterator == 0) {
			final_ejecucion[iterator] = "my script";

			if (token[0] == '$'){
				char eliminado[strlen(token)-1];
				for (int i = 1; i < strlen(token); i++){
					eliminado[i-1] = token[i];
				}

				if (getenv(eliminado) != NULL){
					
					for (int i = 0; i < num_paths; i ++){
						snprintf(ruta_especifica, MAX_COMANDO, "%s/%s", lista_path[i], getenv(eliminado));
						
						if (access(ruta_especifica, R_OK) == 0){
							break;
						}
					}
				}else {
					fprintf(stderr, "error: var %s does not exist\n", eliminado);
				}
			}else {
				for (int i = 0; i < num_paths; i ++){
					snprintf(ruta_especifica, MAX_COMANDO, "%s/%s", lista_path[i], token);
					
					if (access(ruta_especifica, R_OK) == 0){
						break;
					}
				}
			}			
		} else {
			if (token[0] == '$'){
				char eliminado[strlen(token)-1];

				for (int i = 1; i < strlen(token); i++){
					eliminado[i-1] = token[i];
				}
				if (getenv(eliminado) != NULL){
					final_ejecucion[iterator] = getenv(eliminado);
				}else {
					fprintf(stderr, "error: var %s does not exist\n", eliminado);
				}
			}else {
				final_ejecucion[iterator] = token;
			}
		}
		final_ejecucion[iterator + 1] = NULL;
		iterator++;
	}
	
	liberar_argumentos(lista_path, num_paths);

	// fprintf(stderr, "RUTA_BIN:%s RUTA USR: \n", ruta_especifica);
	execv(ruta_especifica, final_ejecucion);
}

//Creamos un proceso hijo y esperamos a que este muera para crear otro hijo
void
fork_comandos(char *comandos )
{

	pid_t child_pid, wtr;
	int wstatus;
	child_pid = fork();
	switch (child_pid) {
	case -1:
		err(EXIT_FAILURE, "fork failed!");
	case 0:
		
		funciones(comandos);
		err(1, "exec failed");
	default:
		wtr = waitpid(child_pid, &wstatus, 0);
		if (wtr == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
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
	int to_read = SIZE;
	char buf[SIZE];
	int no_leido;

	read(0, buf, to_read);
	//fprintf(stderr, "buff: %s\n", buf);
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
	int status, num_pipes, num_funciones;

	num_pipes = numero_entradas - 1;
	num_funciones = numero_entradas;

	int **pipes = (int**)malloc((num_pipes)* sizeof(int *));
	int *childs = (int*)malloc((num_funciones)* sizeof(int));
	crear_pipes(pipes, num_pipes);
    int i;
    int child;
	for(i=0;i<numero_entradas;i++){
				
				//Se??ales, accion por defecto
				signal(SIGINT , SIG_DFL);
				signal(SIGQUIT, SIG_DFL);
				
				
				
					// Hace un fork() para cada hijo. El numero va cambiando con los forks
					//childs[i]=fork();		
                    child = fork();		
					if(child<0){ //Error fork
						fprintf(stderr, "Fall?? el fork().\n%s\n", strerror(EXIT_FAILURE));
						exit(1);
					} else if (childs[i]==0){ // Sino error hace hijos

						if(i==0){ //Primer hjijo
							close(pipes[i][0]);
							dup2(pipes[i][1],1);
							funciones(entradas[i]);

							fprintf(stderr, "Error al ejecutar el comando: %s\n", strerror(EXIT_FAILURE));
							exit(1);
							
						}else if(i==(numero_entradas-1)){	// Utimo hijo
							close(pipes[i-1][1]);
							dup2(pipes[i-1][0],0);
							funciones(entradas[i]);

							fprintf(stderr, "Error al ejecutar el comando: %s\n", strerror(EXIT_FAILURE));
							exit(1);
							
						}else{	// Resto de hijos			
							close(pipes[i][0]);	
							close(pipes[i-1][1]);	
							dup2(pipes[i-1][0],0);	
							dup2(pipes[i][1],1);

							funciones(entradas[i]);

							fprintf(stderr, "Error al ejecutar el comando: %s\n", strerror(EXIT_FAILURE));
							exit(1);
							
						}

					}else{	//Padre
						if(!(i==(numero_entradas-1))){ // Por esto la violacion del core
							close(pipes[i][1]);
						}
					}
						
			}
	waitpid(child,&status,0);
	//wait(&status);
	if (close(pipes[numero_entradas-2][READ_END]) == -1){
		err(EXIT_FAILURE,"close failed");
	}
	//Liberamos toda la memoria utilizada
	liberar_pipes(pipes, numero_entradas - 2);
	free(pipes);
	free(childs);
}

void 
init_pipelines(char * lineas_pipes)
{
	int num_pipes = 0;
	char separar_pipes[2] = "|";

	num_pipes = get_num_tokens(lineas_pipes, separar_pipes);

	char **argumentos = (char**)malloc(num_pipes*sizeof(char *));
	
	crear_string(lineas_pipes, argumentos, num_pipes, separar_pipes);

	pipelines(num_pipes, argumentos);
	liberar_argumentos(argumentos, num_pipes);
	free(argumentos);
}


void 
change_directory(char * directorio){
    int error;
  
    // using the command
    error = chdir(directorio);
    if (error != 0 ){
        perror("chdir failed");
    }

}

void 
mirar_utilidad(char *buff)
{
	int longitud = strlen(buff);
	char *rest_1, buffer_mandar[longitud], buffer_real[longitud], buffer_copia[longitud], builtin[longitud]; 
	char *rest_2, *token_1, *token_2, *builtin_copy;

	snprintf(buffer_mandar, longitud, "%s", buff);
	snprintf(buffer_real, longitud, "%s", buff);
	snprintf(buffer_copia, longitud, "%s", buff);
	snprintf(builtin, longitud, "%s", buff);
	//strcat(buffer_mandar, "\n");
	//strcat(buffer_real, "\n");
	rest_1 = buffer_copia;
	token_1 = strtok_r(rest_1, "=", &rest_1);
	rest_2 = buffer_mandar;
	token_2 = strtok_r(rest_2, "|", &rest_2);
	
	if (strcmp(token_1, buffer_real) != 0){
		//fprintf(stderr, "Estamos en un modo de asignar variables de estado\n");
		set_variables_entorno(buffer_real);
	}else if (strcmp(token_2, buffer_real) != 0){
		
		//fprintf(stderr, "MODO PIPES\n");
		init_pipelines(buffer_real);
	}else {
		builtin_copy = builtin;
		token_1 = strtok_r(builtin_copy, " ", &builtin_copy);
		token_2 = strtok_r(builtin_copy, " ", &builtin_copy);
		if (strcmp(token_1, "cd") == 0){
			if (token_2 == NULL){
				change_directory(getenv("HOME"));
			}else {
				change_directory(token_2);
			}	
			
		}else {
			//fprintf(stderr, "COMANDOS NORMALES\n");
			fork_comandos(buffer_real);
		}

		
	}
}

int
main(int argc, char *argv[])
{
	while(1)
    {
        char buf[MAX_STDIN];
		printf("<> ");
        fgets(buf, MAX_STDIN, stdin);

		mirar_utilidad(buf);
		//fork_comandos(buf);
        //printf("string is: %s\n", buf);
    }
}
