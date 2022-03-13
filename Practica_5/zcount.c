#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

void 
escritura_ceros(int fd2, long counter, char * fichero)
{
	char buf[sizeof(long)];
	int no_leido = 0;
	int no_escrito;
	char * linea;
	sprintf(buf, "%ld", counter);

	linea = (char *)malloc(((strlen(fichero) + strlen(buf)) * sizeof(char *)));
	strcpy(linea, buf);
	strcat(linea, " ");
	strcat(linea, fichero);
	strcat(linea, "\n");

	printf("NUMERO DE CEROS en long %ld y en string: %s y se queda la linea: %s\n ", counter, buf, linea);
	no_escrito = write(fd2, buf, no_leido);

	if (no_escrito != no_leido){
		warn("cant write");
	}
	free(linea);
}

long 
lectura_ceros(int fd1)
{
	long counter = 0;
	int to_read = 1, no_leido = 0;
	char buf[1];
	int done = 0;

	

	while (!done){
		no_leido = read(fd1, buf, to_read);
		if (no_leido < 0){
			err(1, "can't read input file");
		}else if (no_leido == 0){
			done = 1;
		}else {
			//printf("BUFF %s      ", buf);
			if (strcmp(buf, "\0") == 0){
				counter +=1;
			}
			
		}
	}
	
	//printf("CONTADOR CEROS %ld \n", counter);
	return counter;
}

void
read_directory(char *directorio)
{
	/* Variables */
	DIR *dirp;
	struct dirent *direntp;
	int fd1, fd2;
	long counter;
	//size_t resultado;
	char z[6] = "z.txt";
	char fichero_final[strlen(directorio + 6)];
	char *ayuda = NULL;
	strcat(strcpy(fichero_final, directorio), z);

	printf("DIRECTORIO FINAL: %s\n", fichero_final);

	fd2 = open(fichero_final, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd2 == -1) {
		err(1, "can't open output file");
	}

	write(fd2, "ahsdfhaksdfh", 0);
	/* Abrimos el directorio */
	dirp = opendir(directorio);
	if (dirp == NULL){
		printf("Error: No se puede abrir el directorio\n");
		exit(2);
	}
	
	while ((direntp = readdir(dirp)) != NULL) {
		if (direntp->d_name[0] != '.' && direntp->d_name[0] != 'z'){
			ayuda = (char *)malloc(((strlen(direntp->d_name) + strlen(directorio)) * sizeof(char *)));
			strcat(strcpy(ayuda, directorio), direntp->d_name);
			fprintf(stderr, "FICHERO A ABRIR %s jajajjaj\n",  ayuda);
			fd1 = open( ayuda, O_RDONLY);
			if (fd1 == -1) {
				err(1, "can't open input file: %s", direntp->d_name);
			}
			counter = lectura_ceros(fd1);
			escritura_ceros(fd2, counter, direntp->d_name);
		}

		free(ayuda);
	}
	/* Cerramos el directorio */
	closedir(dirp);
}



int main (int argc, char *argv[]){
 
	

	/* Comprobamos los argumentos */
	if (argc != 2){
		fprintf(stderr, "Introduce el número de argumentos correctos\n");
		exit(1);
	}

	read_directory(argv[1]);

}
