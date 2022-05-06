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

char *
get_command(char *script) {

	char *token;
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
    fprintf(stderr, "MAYOR: %d, MENOR: %d\n",mayor, menor );
    if (menor < mayor || mayor == 0 ){
        printf("arriba\n");
        token = strtok_r(rest, "<", &rest);
    }else if (menor > mayor || menor ==  0 ){
        printf("abajo\n");
        token = strtok_r(rest, ">", &rest);
    }
}

int to_read = SIZE;
char buf[SIZE];
fprintf(stderr, "HOLA ANTES\n");
read(fd_salida[READ_END], buf, to_read);
fprintf(stderr, "buf: %s\n", buf);
fprintf(stderr, "HOLA DESPUES\n");
int
main(int argc, char *argv[])
{    
    //char prueba[8] = "$pepel";
    //char eliminado[strlen(prueba-1)];
//
    //elimina_caracter(prueba, eliminado);
//
    //printf("ORIGINAL:%s   , MANEJADO:%s \n", prueba, eliminado);
    
    get_command(argv[1]);


    printf("%s\n", argv[1]);
    return 0;
}