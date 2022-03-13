#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

enum {

	MENOR_CERO = -1,
	NUMERO_INCORRECTO = -2,
	SIZE = 8 * 1024,
};

int
copy(int desc_entrada, int desc_salida, int count_bytes)
{
	int to_read = SIZE;
	char buf[SIZE];
	int no_leido;
	int done = 0;
	int total = 0, no_escrito = 0;

	while (!done) {
		to_read = SIZE;
		if (count_bytes != -1) {	//&& count_bytes - total < SIZE){
			to_read = count_bytes - total;
		}
		no_leido = read(desc_entrada, buf, to_read);
		//printf("BYTES A LEER  %d y count bytes %d y done %d to read %d\n", to_read, count_bytes, done, no_leido);
		if (no_leido < 0) {
			err(1, "can't read input file");
		} else if (no_leido == 0) {
			//printf("HOLAAAA\n");
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

int
openfile(char *fichero_entrada, char *fichero_salida, int count_bytes)
{
	int desc_entrada = 0;	// Si ponemos el desciptor de ficheros a 0 la entrada estandar será por la terminal
	int desc_salida = 1;	// Si ponemos el descriptor de ficheros un 1 la salida será por la salida estandar (terminal)
	int salida_copy = 0;

	if (strcmp(fichero_entrada, "-") == 0) {
		desc_entrada = 0;
	} else {
		desc_entrada = open(fichero_entrada, O_RDONLY);
		if (desc_entrada == -1) {
			err(1, "can't open input file");
		}
	}
	if (strcmp(fichero_salida, "-") == 0) {
		desc_salida = 1;
	} else {
		desc_salida =
		    open(fichero_salida, O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (desc_salida == -1) {
			err(1, "can't open output file");
		}
	}
	salida_copy = copy(desc_entrada, desc_salida, count_bytes);

	close(desc_salida);
	close(desc_entrada);

	return salida_copy;
}

//Comprobamos que el parametro que nos pasan por linea de comandos sea un numero
long
get_num_bytes(char *numero)
{
	long num_bytes;
	char *ptr;

	num_bytes = strtol(numero, &ptr, 10);

	if (strcmp(ptr, "\0") != 0) {
		num_bytes = NUMERO_INCORRECTO;
	} else {
		if (num_bytes <= 0) {
			num_bytes = MENOR_CERO;
		}
	}
	printf("%ld\n", num_bytes);
	return num_bytes;
}

int
main(int argc, char *argv[])
{
	int copy_bytes = -1;
	int error = 0;

	if (argc <= 2 || argc > 4) {
		fprintf(stderr,
			"usage: ch varname varcontent [varname varcontent] ...\n");
		exit(EXIT_FAILURE);
	}
	//Tenemos un número para poder pasar un numero en concreto
	if (argc == 4) {
		copy_bytes = get_num_bytes(argv[argc - 1]);
		error = openfile(argv[1], argv[2], copy_bytes);
		if (error < 0) {
			err(1, "can't write");
		}
	} else {
		error = openfile(argv[1], argv[2], copy_bytes);
		if (error < 0) {
			err(1, "can't write");
		}
	}
	exit(EXIT_SUCCESS);
}
