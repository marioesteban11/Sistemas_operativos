#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

enum {

	SIZE = 8 * 1024,
	FALLO_LEER_DIRECTORIO = -1,
	FALLO_STAT = -2,
};

struct cantidad {
	int bytes;
	int ficheros_c;
	int ficheros_h;
	int error;
};

void
salida(int error)
{
	if (error == 0) {
		exit(EXIT_SUCCESS);
	} else {
		exit(EXIT_FAILURE);
	}
}

struct cantidad
analizar_fichero(char *fichero, struct stat SMetaData,
		 struct cantidad *cantidad)
{
	char *final_fichero;
	char punto = '.';

	final_fichero = strrchr(fichero, punto);
	if (final_fichero != NULL) {
		if (strcmp(final_fichero, ".c") == 0) {
			cantidad->bytes += SMetaData.st_size;
			cantidad->ficheros_c++;
		} else if (strcmp(final_fichero, ".h") == 0) {
			cantidad->bytes += SMetaData.st_size;
			cantidad->ficheros_h++;
		}
	}
	return *cantidad;
}

struct cantidad
read_directory(char *directorio, struct cantidad *cantidad)
{
	// Variables
	DIR *dirp;
	struct stat SMetaData;
	struct dirent *direntp;
	char *fichero_final;
	// Abrimos el directorio
	//fprintf(stderr,"DIRECTORIO: %s\n", directorio);
	dirp = opendir(directorio);
	if (dirp == NULL) {
		cantidad->error = FALLO_LEER_DIRECTORIO;
		fprintf(stderr, "Error: No se puede abrir el directorio, %s\n", directorio);
		//exit(EXIT_FAILURE);
		return *cantidad;
	}
	while ((direntp = readdir(dirp)) != NULL) {

		if (direntp->d_name[0] != '.') {

			fichero_final =
			    (char *)
			    malloc((strlen(direntp->d_name) +
				    strlen(directorio)) * sizeof(char *));

			sprintf(fichero_final, "%s/%s", directorio,
				direntp->d_name);
			//snprintf(fichero_final, sz, "%s/%s", d, f);
			//fprintf(stderr, "COSA A LEER: %s\n", fichero_final);
			if (stat(fichero_final, &SMetaData) == -1) {
				cantidad->error = FALLO_STAT;
				perror("stat");
				fprintf(stderr, "En el fichero: %s\n",
					fichero_final);
			} else {
				//Si es un fichero
				if ((SMetaData.st_mode & S_IFMT) == S_IFREG) {
					analizar_fichero(direntp->d_name,
							 SMetaData, cantidad);
				} else if ((SMetaData.st_mode & S_IFMT) ==
					   S_IFDIR) {
					//fprintf(stderr, "Es un directorio %s \n", direntp->d_name);
					read_directory(fichero_final, cantidad);
				}
			}

			free(fichero_final);
		}
	}
	//printf("\n");
	//printf("%s\t%d\t%d\t%d\n", directorio, cantidad->ficheros_c, cantidad->ficheros_h, cantidad->bytes);
	//Cerramos el directorio 
	closedir(dirp);
	return *cantidad;
}

int
encontrar_directorios()
{
	char bufer[SIZE];	// Aquí vamos a ir almacenando cada línea
	struct cantidad cantidad;
	char *token;

	cantidad.error = 0;
	while (fgets(bufer, SIZE, stdin)) {

		cantidad.bytes = 0;
		cantidad.ficheros_c = 0;
		cantidad.ficheros_h = 0;
		char *rest = bufer;

		token = strtok_r(rest, "\n", &rest);
		read_directory(token, &cantidad);

		printf("%s\t%d\t%d\t%d\n", bufer, cantidad.ficheros_c,
		       cantidad.ficheros_h, cantidad.bytes);
	}
	return cantidad.error;
}

int
main(int argc, char *argv[])
{
	int error = 0;

	if (argc != 1) {
		fprintf(stderr,
			"Introduce el número de argumentos correctos\n");
		exit(1);
	}

	error = encontrar_directorios();

	salida(error);
}
