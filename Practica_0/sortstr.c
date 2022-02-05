#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
es_vocal(char *palabra)
{
	//printf("PALABRA A DESMENUZAR %s, y su sizeof %ld\n", palabra, strlen(palabra));
	int vocal = 0;

	for (int j = 0; j < strlen(palabra); j++) {
		//printf("%c", palabra[j]);

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
	//printf("  %d, vocales\n", vocal);
	return vocal;
}

void
insertion_sort(char *palabras[], int longitud)
{
	char *key;
	int i, j;

	for (i = 2; i < longitud; i++) {
		key = palabras[i];
		j = i - 1;
		//Devuelve 0 si es igual, -algo si la palabra primera es menor(va antes) que la segunda
		// y numero positivo si lo primero es mayor que lo segundo
		while (j >= 0 && strcmp(palabras[j], key) > 0) {
			palabras[j + 1] = palabras[j];
			j--;
		}
		palabras[j + 1] = key;
	}

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
		if (es_vocal(argv[i]) > 0) {
			printf("%s\n", argv[i]);
		}
	}
	exit(EXIT_SUCCESS);
}
