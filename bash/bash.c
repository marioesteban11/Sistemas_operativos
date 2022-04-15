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
};


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

//Creamos un proceso hijo y esperamos a que este muera para crear otro hijo
void
fork_comandos(char *comandos )
{

	pid_t child_pid, wtr;
	int i;
	int wstatus;

	for (i = 0; i < 1; i++) {
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
			if (wstatus != 0) {
				fprintf(stderr, "error %s\n", comandos);
				exit(EXIT_FAILURE);
			}
		}
	}
}

void 
mirar_utilidad(char *buff)
{
	int longitud = strlen(buff);
	char *rest_1 = buff, buffer_mandar[longitud]; 
	char *rest_2 = buff , *token, *pelele;

	snprintf(buffer_mandar, sizeof(buffer_mandar), "%s", buff);

	token = strtok_r(rest_1, "=", &rest_1);
	fprintf(stderr, "CON REST_1 es: %s y rest es: %s , buff es: %s y la comparacion es: %d\n", token, rest_1, buffer_mandar, strcmp(token, buffer_mandar));

	pelele = strtok_r(rest_2, "|", &rest_2);

	fprintf(stderr, "RESTTTTTT DOOOOOOOOOOS es: %s y rest es: %s , buff es: %s y la comparacion es: %d\n", pelele, rest_2, buffer_mandar, strcmp(pelele, buffer_mandar));
	//if (strcmp(strtok_r(rest_1, "=", &rest_1), buffer_mandar) != 0){
	//	fprintf(stderr, "Estamos en un modo de asignar variables de estado\n");
	//}else if (strcmp(strtok_r(rest_2, "|", &rest_2), buffer_mandar) != 0){
	//	fprintf(stderr, "MODO PIPES\n");
	//}else {
	//	fprintf(stderr, "COMANDOS NORMALES\n");
	//}


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
