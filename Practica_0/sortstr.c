#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Comprobamos si almenos una letra de nuestra palabra es una vocal
int
es_vocal(char *palabra)
{
	int vocal = 0;

	for (int j = 0; j < strlen(palabra); j++) {

		if (palabra[j] == 'a' || palabra[j] == 'A') {
			vocal++;
		} else if (palabra[j] == 'e' || palabra[j] == 'E') {
			vocal++;
		} else if (palabra[j] == 'i' || palabra[j] == 'I') {
			vocal++;
		} else if (palabra[j] == 'o' || palabra[j] == 'O') {
			vocal++;
		} else if (palabra[j] == 'u' || palabra[j] == 'U') {
			vocal++;
		}
	}
	return vocal;
}

// realizamos el algoritmo de ordenación de insertion sort
void
insertion_sort(char *palabras[], int longitud)
{
	char *key;
	int i, j;

	for (i = 2; i < longitud; i++) {
		key = palabras[i];
		j = i - 1;
		while (j >= 0 && strcasecmp(palabras[j], key) > 0) {
			palabras[j + 1] = palabras[j];
			j--;
		}
		palabras[j + 1] = key;
	}

}

// Comprobamos si en nuestro string hay algun numero
int
es_numero(char *palabra)
{
	int numero = 0;

	for (int i = 0; i < strlen(palabra); i++) {
		if (palabra[i] >= '0' && palabra[i] <= '9') {
			numero++;
		}
	}
	return numero;
}

int
main(int argc, char *argv[])
{
	if (argc <= 1) {
		fprintf(stderr, "Introduce argmentos\n");
		exit(1);
	}
	insertion_sort(argv, argc);
	for (int i = 1; i < argc; i++) {
		if (es_vocal(argv[i]) > 0 && es_numero(argv[i]) == 0) {
			printf("%s\n", argv[i]);
		}
	}
	exit(EXIT_SUCCESS);
}
