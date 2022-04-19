#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/stat.h>

enum {
    MAX_STDIN = 100,
};

// Valgrind da un error de: ERROR SUMMARY: 25 errors from 3 contexts, de algo de saltos condicionales de bloques
// Preguntar el miercoles
int 
get_num_tokens(char *path, char *separacion)
{
    char * rest_1, *token;
    int longitud = strlen(path), num_tokens = 0;
    char copia_1[longitud];
    snprintf(copia_1, longitud, "%s", path);
    rest_1 = copia_1;

    while ((token = strtok_r(rest_1, separacion, &rest_1))) {
		num_tokens++;
	}
    return num_tokens;
}

void
crear_string(char *path, char **lista_path, int num_pipes)
{
    int longitud = strlen(path), iterator = 0;
    char copia_1[longitud];
    char *rest_1, *token;

    snprintf(copia_1, longitud+1, "%s", path);
    rest_1 = copia_1;
    printf("VARIABLE PATH COPIADA %s\n", rest_1);
    while ((token = strtok_r(rest_1, ":", &rest_1))) {
        char *ayuda = (char*)malloc(strlen(token)*sizeof(char));
        printf("TOKEN: %s\n", token);
        snprintf(ayuda, strlen(token) + 1, "%s", token);
        lista_path[iterator] = ayuda;
        iterator++;
	}
}

char 
**conseguir_paths()
{
    char *path; //*rest_1, *rest_2, *token;
    int num_pipes = 0;// iterator = 0;
    char separar_path[2] = ":";
    path = getenv("PATH");

    num_pipes = get_num_tokens(path, separar_path);
    printf("%s\n", path);
    char **lista_path = (char**)malloc(num_pipes*sizeof(char*));

    crear_string(path, lista_path, num_pipes);

    for (int i = 0; i < num_pipes; i ++){
        printf("%s\n", lista_path[i]);
    }
    for (int i = 0; i < num_pipes; i ++){
        free(lista_path[i]);
    }
    free(lista_path);
    return lista_path;
}

int
main(int argc, char *argv[])
{    
    conseguir_paths();
    return 0;
}