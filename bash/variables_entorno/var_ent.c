#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/stat.h>


void set_variables_entorno(char * expresion){

    char* token;
    char* rest = expresion;
	char * enviroment;
 
	token = strtok_r(rest, "=", &enviroment);

    //while ((token = strtok_r(rest, "=", &rest)))
    //    printf("%s\n", token);

	setenv(token, rest, 1);

	printf("VARIABLE DE ENTORNO: %s valor de la variable de entornos %s \n",token, getenv(token));

	printf("CHAR PPAL: %s, char secundario %s y rest %s y environment %s\n", expresion, token, rest, enviroment );
}


int
main(int argc, char *argv[])
{
	

	set_variables_entorno(argv[1]);

    
}
