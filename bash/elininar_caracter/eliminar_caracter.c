#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


void 
elimina_caracter(char *original, char * eliminado){
    int longitud = strlen(original);

    //char eliminado[longitud];
    for (int i = 1; i < longitud; i++){
        eliminado[i-1] = original[i];
    }
}

int
main(int argc, char *argv[])
{    
    char prueba[8] = "$pepel";
    char eliminado[strlen(prueba-1)];

    elimina_caracter(prueba, eliminado);

    printf("ORIGINAL:%s   , MANEJADO:%s \n", prueba, eliminado);
    return 0;
}