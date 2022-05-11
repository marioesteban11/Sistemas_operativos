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

struct redireccion
{
	int fd;
	char restante[MAX_COMANDO];
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
	fprintf(stderr, "STRING QUE LLEGA :%s, y lo que hay que separar:   %s\n", path, separacion);
    char * rest_1, *token;
    int longitud = strlen(path), num_tokens = 0;
    char copia_1[longitud];
    snprintf(copia_1, longitud+1, "%s", path);
    rest_1 = copia_1;
	fprintf(stderr, "\nREST: %s\n", strtok_r(rest_1, separacion, &rest_1));
    while ((token = strtok_r(rest_1, separacion, &rest_1))) {
		num_tokens++;
	}

    return num_tokens;
}

int
copy(int desc_entrada, int desc_salida)
{
	int to_read = SIZE;
	char buf[SIZE];
	int no_leido;
	int done = 0;
	int total = 0, no_escrito = 0;
	int count_bytes = -1;

	while (!done) {
		to_read = SIZE;
		if (count_bytes != -1) {
			to_read = count_bytes - total;
		}
		no_leido = read(desc_entrada, buf, to_read);
		if (no_leido < 0) {
			err(1, "can't read input file");
		} else if (no_leido == 0) {
			done = 1;
		} else {
			no_escrito = write(desc_salida, buf, no_leido);
			if (no_escrito != no_leido) {
				warn("cant write");
			}
			total += no_leido;
			done = count_bytes != -1 && count_bytes == total;
		}

	}
	return total;
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
}


struct redireccion 
get_dev_null(char * script)
{
	struct redireccion dev_null;
	
	char *final_fichero;
	char *rest = script;
	char dev = '&';
	char *dev_pointer = "&";
	char *token;
	final_fichero = strrchr(script, dev);
	int fd = 1;
	token = strtok_r(rest, dev_pointer, &rest);
	if (final_fichero != NULL) {
		fd = open("/dev/null", O_WRONLY , 0664);
		snprintf(dev_null.restante, MAX_COMANDO, "%s", dev_pointer);
		if (fd == -1){
			fprintf(stderr, "error al abrir el fichero dev_null\n");
		}
	}else {
		snprintf(dev_null.restante, MAX_COMANDO, "%s", token);
	}

	dev_null.fd = fd;
	
	return dev_null;
}



struct redireccion
detect_fichero_salida(char *script)
{
	struct redireccion fichero;
	char *final_fichero;
	char *rest = script;
	char * token;
	char dev = '>';
	char *dev_pointer = ">";


	final_fichero = strrchr(script, dev);
	//fprintf(stderr, "LO QUE ENTRA EN SALIDA:%s\n", script);
	token = strtok_r(rest, dev_pointer, &rest);
	token = strtok_r(rest, " ", &rest);
	
	int fd = 1;
	//fprintf(stderr, "TOKENSITO: %s\n", token);
	if (final_fichero != NULL) {
		fd = open(token, O_WRONLY |O_CREAT | O_TRUNC, 0664);
		if (fd < 0){
			fprintf(stderr, "error al abrir el fichero de salida\n ");
		}
	}
	fichero.fd = fd;
	snprintf(fichero.restante, MAX_COMANDO, "%s", token);
	
	return fichero;
}

struct redireccion
detect_fichero_entrada(char *script)
{
	struct redireccion fichero;
	char *final_fichero;
	char *rest = script;
	char * token;
	char dev = '<';
	char *dev_pointer = "<";
	//fprintf(stderr, "SCRIPT:%s\n", script);
	final_fichero = strrchr(script, dev);
	token = strtok_r(rest, dev_pointer, &rest);
	token = strtok_r(rest, " ", &rest);
	int fd = 1;
	if (final_fichero != NULL){
		fd = open(token, O_RDONLY);
		//fprintf(stderr, "fd salida: %d, con token: %s\n", fd, token);
		if (fd == -1){
			fprintf(stderr, "error al abrir el fichero de entrada\n");
		}
	}
	fichero.fd = fd;
	snprintf(fichero.restante, MAX_COMANDO, "%s", token);
	return fichero;
}

void
get_command(char *script) 
{

    char *rest = script;
    int mayor = 1000, menor = 1000;
    //token = strtok_r(rest, ">", &rest);
    
    for (int i = 0; i < strlen(script); i++)
    {
        if (script[i] == '<'){
            menor = i;
        }
        if (script[i] == '>'){
            mayor = i;
        }
    }
    if (menor < mayor || mayor == 0 ){
        strtok_r(rest, "<", &rest);
    }else if (menor > mayor || menor ==  0 ){
        strtok_r(rest, ">", &rest);
    }
}

void
exec_comando(char *script)
{
	char *final_ejecucion[TOKENS];

	char *rest = script, *token;
	int iterator = 0;
	char ruta_especifica [MAX_COMANDO];
	char *path; 
	char separar_path[2] = ":";
	path = getenv("PATH");
	int num_paths = 0;

	num_paths = get_num_tokens(path, separar_path);
    //printf("%s\n", path);
    char **lista_path = (char**)malloc(num_paths*sizeof(char*));
	//fprintf(stderr, "script\t%s\n", script);
	crear_string(path, lista_path, num_paths, separar_path);

	//fprintf(stderr, "Num paths %d\n", num_paths);
	//for (int i = 0; i < num_paths; i++){
	//	fprintf(stderr, "lista path[%d]: %s\n", i, lista_path[i]);
	//}


	while ((token = strtok_r(rest, " ", &rest))) {
		
		//printf("%s ", rest);
		if (iterator == 0) {
			final_ejecucion[iterator] = script;

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
					//fprintf(stderr, "SCRIPT dentro: %s y los paths: %s\n", script, lista_path[i]);
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

	//fprintf(stderr, "RUTA_BIN:%s RUTA USR: \n", ruta_especifica);
	execv(ruta_especifica, final_ejecucion);
}

void
cerrar_descriptores(int dev_null, int fichero_entrada, int fichero_salida)
{
	if (dev_null != 1){
		if(close(dev_null) < 0){
			perror("Error al cerrar fichero dev_null");
		}
	}
	if (fichero_entrada != 1){
		if(close(fichero_entrada) < 0){
			perror("Error al cerrar fichero entrada");
		}
	}
	if (fichero_salida != 1){
		if(close(fichero_salida) < 0){
			perror("Error al cerrar fichero salida");
		}
	}
}
//Creamos un proceso hijo y esperamos a que este muera para crear otro hijo
void
fork_comandos(char *comandos )
{
	pid_t child_pid, wtr;
	struct redireccion dev_null;
	struct redireccion fichero_salida;
	struct redireccion fichero_entrada;
	int fd[2], fd2[2];
	int wstatus;
	char copia_entrada[strlen(comandos)]; 
	char copia_salida[strlen(comandos)];

	snprintf(copia_salida, strlen(comandos)+1, "%s", comandos);
	snprintf(copia_entrada, strlen(comandos)+1, "%s", comandos);
	dev_null = get_dev_null(comandos);
	fichero_salida = detect_fichero_salida(copia_salida);
	fichero_entrada = detect_fichero_entrada(copia_entrada);


	if(pipe(fd) < 0){
		err(EXIT_FAILURE, "cannot make a pipe");
	}
	if(pipe(fd2) < 0){
		err(EXIT_FAILURE, "cannot make a pipe");
	}

	child_pid = fork();
	switch (child_pid) {
	case -1:
		err(EXIT_FAILURE, "fork failed!");
	case 0:
		if (fichero_salida.restante != NULL && fichero_salida.fd != 1){
			get_command(copia_salida);
			comandos = copia_salida;
			close(fd2[READ_END]);
			dup2(fd2[WRITE_END], WRITE_END);
			close(fd2[WRITE_END]);
		}else {
			close(fd2[READ_END]);
			close(fd2[WRITE_END]);
		}

		if (fichero_entrada.restante != NULL && fichero_entrada.fd != 1){
			get_command(copia_entrada);
			comandos = copia_entrada;
			close(fd[WRITE_END]);
			dup2(fd[READ_END], READ_END);
			close(fd[READ_END]);
		}else {
			close(fd[READ_END]);
			close(fd[WRITE_END]);
		}
		
		if (strcmp(dev_null.restante, comandos) != 0)
		{
			dup2(dev_null.fd, WRITE_END);
		}
		
		exec_comando(comandos);
		err(1, "exec failed");
	default:
		if (fichero_entrada.restante != NULL && fichero_entrada.fd != 1){
			close(fd[READ_END]);
			copy(fichero_entrada.fd, fd[WRITE_END]);
			close(fd[WRITE_END]);
		}else{
			close(fd[WRITE_END]);
			close(fd[READ_END]);
		}
		
		if (fichero_salida.restante != NULL && fichero_salida.fd != 1){
			close(fd2[WRITE_END]);
			//dup2(fd2[READ_END], READ_END);
			copy(fd2[READ_END], fichero_salida.fd);
			close(fd2[READ_END]);
		}else{
			close(fd2[WRITE_END]);
			close(fd2[READ_END]);
		}
		
		wtr = waitpid(child_pid, &wstatus, 0);
		
		if (wtr == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
	}
	
	cerrar_descriptores(dev_null.fd, fichero_entrada.fd, fichero_salida.fd);
	
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
pipe_inicial(int **pipes, int i, int numero_pipes)
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
	int status, num_pipes, num_funciones;
	int pid;
	struct redireccion dev_null;
	struct redireccion fichero_salida;
	struct redireccion fichero_entrada;
	int longitud_ult_elem = strlen(entradas[numero_entradas-1]);

	char copia_entrada[longitud_ult_elem]; 
	char copia_salida[longitud_ult_elem];

	snprintf(copia_salida, longitud_ult_elem+1, "%s", entradas[numero_entradas-1]);
	snprintf(copia_entrada, longitud_ult_elem+1, "%s", entradas[numero_entradas-1]);
	fichero_entrada = detect_fichero_entrada(copia_entrada);
	fichero_salida = detect_fichero_salida(copia_salida);
	dev_null = get_dev_null(entradas[numero_entradas - 1]);
	fprintf(stderr, "FICHERO entrada: %s con fd: %d y cosas salida: %s y %d\n", fichero_entrada.restante, fichero_entrada.fd, fichero_salida.restante, fichero_salida.fd);
	num_pipes = numero_entradas - 1;
	num_funciones = numero_entradas;
	int **pipes = (int**)malloc((num_pipes)* sizeof(int *));
	int *childs = (int*)malloc((num_funciones)* sizeof(int));
	crear_pipes(pipes, num_pipes);
	
	for (int i = 0; i < num_funciones; i++){
    	childs[i] = fork(); 
		switch(childs[i]){
			case -1:
				err(EXIT_FAILURE, "fork failed");
			case 0:
				if(i == 0 ) {
					if (fichero_entrada.restante != NULL && fichero_entrada.fd != 1){
						get_command(copia_salida);
						snprintf(entradas[i], strlen(copia_salida)+1, "%s", copia_salida);
						for (int j = 1; j < num_pipes ; j ++){
							close(pipes[j][READ_END]);
							close(pipes[j][WRITE_END]);
						}
						
						dup2(fichero_entrada.fd, READ_END);
						close(pipes[i][READ_END]);
						dup2(pipes[i][WRITE_END], STDOUT_FILENO); 
						close(pipes[i][WRITE_END]);
					}else {
						pipe_inicial(pipes, i, num_pipes);
					}
				}else if (i == num_pipes){
					if (fichero_salida.restante != NULL && fichero_salida.fd != 1){
						get_command(copia_salida);
						snprintf(entradas[i], strlen(copia_salida)+1, "%s", copia_salida);
						dup2(fichero_salida.fd, WRITE_END);
					}
					if (strcmp(dev_null.restante, entradas[i]) != 0){
						dup2(dev_null.fd, WRITE_END);
					}else if(fichero_entrada.restante != NULL && fichero_entrada.fd != 1){
						get_command(copia_salida);
						snprintf(entradas[i], strlen(copia_salida)+1, "%s", copia_salida);
					}
					pipe_final(pipes, i);
				}else {
					pipes_intermedios(pipes, i, num_pipes);
				}
						fprintf(stderr, "bon voyage\n");

				cerrar_descriptores(dev_null.fd, fichero_entrada.fd, fichero_salida.fd);
				exec_comando(entradas[i]);
		}
	}
	cerrar_descriptores(dev_null.fd, fichero_entrada.fd, fichero_salida.fd);
	for (int i = 0; i < num_pipes; i++){
		if(close(pipes[i][0]) < 0){
			perror("Error al cerrar fichero pipes[0]\n");
		}
		if(close(pipes[i][1]) < 0){
			perror("Error al cerrar fichero pipes[1]\n");
		}
	}
	
	for (int i = 0; i < num_funciones; i++){
		pid = waitpid(childs[i], &status, 0);
		if (pid == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
	}
	
	
	//Liberamos toda la memoria utilizada
	liberar_pipes(pipes, num_pipes);
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
	char eliminado[strlen(directorio)   -   1];
	fprintf(stderr, "askodfoasdfa dfa sd: %ld y ek otro %ld\n", strlen(eliminado), strlen(directorio));

    // using the command
	if (directorio[0] == '$'){
		
		for (int i = 1; i < strlen(directorio); i++){
			eliminado[i-1] = directorio[i];
		}
		fprintf(stderr, "ELIMINADO: %s y tamaño: %ld\n", eliminado, strlen(eliminado));
		if (getenv(eliminado) != NULL){
			error = chdir(eliminado);
		}else {
			perror("no se pudo cambiar de directorio");
		}
	}else {
		error = chdir(directorio);
	}
    
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
	if (token_1 == NULL){
		token_1 = "a";
	}
	if (token_2 == NULL){
		token_2 = "a";
	}
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
		
    }
}