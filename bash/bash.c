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
	BIT_LEER = 1,
	Z_LEN = 6,
	SIZE = 8 * 1024,
};

void
escritura_ceros(int fd2, long long counter, char *fichero)
{
	char buf[sizeof(long long)];
	int no_leido;
	int no_escrito;
	char *linea;

	sprintf(buf, "%lld", counter);

	//no_escrito = write(fd2, "dyrtfughijfdsetdf dsetdfgjhk", no_leido);
	linea =
	    (char *)malloc(((strlen(fichero) + strlen(buf)) * sizeof(char *)));
	strcpy(linea, buf);
	strcat(linea, "\t");
	strcat(linea, fichero);
	strcat(linea, "\n");

	//printf("NUMERO DE CEROS en long %lld y en string: %s y se queda la linea: %s \n ", counter, buf, linea);
	no_leido = strlen(linea);
	no_escrito = write(fd2, linea, no_leido);

	//printf("Descriptor de fichero: %d y numero de bytes no escritos: %d\n", fd2, no_escrito);
	if (no_escrito != no_leido) {
		warn("cant write");
	}

	free(linea);
}

long long
lectura_ceros(int fd1)
{
	long long counter = 0;
	int to_read = SIZE, no_leido = 0;
	char buf[SIZE];
	int done = 0;

	while (!done) {
		//to_read = SIZE;
		no_leido = read(fd1, buf, to_read);

		//printf("BYTES A LEER  %d y count bytes %d y done %d to read %d\n", to_read, count_bytes, done, no_leido);
		if (no_leido < 0) {
			err(1, "can't read input file");
		} else if (no_leido == 0) {
			done = 1;
		} else {
			//fprintf(stderr, "BUFF %sy no leido %d y strlen %ld y expresion %d\n",buf, no_leido, strlen(buf), strlen(buf) == 0 );
			for (int i = 0; i < no_leido; i++) {
				if (buf[i] == 0) {
					counter++;
				}
			}
		}

	}
	//printf("CONTADOR CEROS %ld \n", counter);
	return counter;
}

void
read_directory(char *directorio)
{
	// Variables
	DIR *dirp;
	struct dirent *direntp;
	int fd1, fd2;
	long long counter;
	char z[Z_LEN] = "z.txt";
	char fichero_final[strlen(directorio + Z_LEN)];
	char *ayuda = NULL;
	struct stat SMetaData;

	if (strcmp(directorio, ".") == 0) {
		strcpy(fichero_final, z);
	} else if (directorio[strlen(directorio) - 1] != '/') {
		strcat(directorio, "/");
		strcat(strcpy(fichero_final, directorio), z);

	} else {
		strcat(strcpy(fichero_final, directorio), z);
	}

	fd2 = open(fichero_final, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd2 == -1) {
		err(1, "can't open output file");
	}
	// Abrimos el directorio
	dirp = opendir(directorio);
	if (dirp == NULL) {
		printf("Error: No se puede abrir el directorio\n");
		exit(EXIT_FAILURE);
	}

	while ((direntp = readdir(dirp)) != NULL) {

		if (direntp->d_name[0] != '.'
		    && strcmp(direntp->d_name, "z.txt") != 0) {

			if (strcmp(directorio, ".") == 0) {
				ayuda = (char *)
				    malloc((strlen(direntp->d_name) *
					    sizeof(char *)));
				strcpy(ayuda, direntp->d_name);
			} else {
				ayuda = (char *)
				    malloc(((strlen(direntp->d_name) +
					     strlen(directorio)) *
					    sizeof(char *)));
				strcat(strcpy(ayuda, directorio),
				       direntp->d_name);
			}
			stat(ayuda, &SMetaData);
			//fprintf(stderr, "FICHERO A ABRIR %s \n",  ayuda);
			if ((SMetaData.st_mode & S_IFMT) == S_IFREG) {
				fd1 = open(ayuda, O_RDONLY);
				if (fd1 == -1) {
					err(1, "can't open input file: %s",
					    direntp->d_name);
				}
				counter = lectura_ceros(fd1);
				escritura_ceros(fd2, counter, direntp->d_name);
				//fprintf(stderr, "FALLO en otra funcion\n");
			}
			free(ayuda);
		}
	}
	//Cerramos el directorio 
	closedir(dirp);
}

int
main(int argc, char *argv[])
{

	// Comprobamos los argumentos
	if (argc != 2) {
		fprintf(stderr,
			"Introduce el número de argumentos correctos\n");
		exit(1);
	}

	read_directory(argv[1]);

}
