#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "stack.h"


//Puntero array, numero de elementos maximos y numero de elementos actuales
// Si nos quedamos sin memoria se usa realloc
Stack * newstack(int nelem)
{
	void ** array_elem = (void **)malloc(nelem*sizeof(void*));

	Stack * puntero_stack = (Stack*)malloc( sizeof(Stack) );
	Stack *stack;

	stack->max_elem = nelem;
	stack->nelem = 0;
	stack->elementos = array_elem;

	return stack;

}

// Si hay al menos un elemento la funcion devolvera false
int is_empty(Stack *s)
{
	if (s->nelem == 0){
		return -1;
	}else {
		return 0;
	}
}

// Devuelve el numero de elementos de la pila;
int nelem(Stack *s)
{
	return s->nelem;
}

// Inserta un elemento en la cola (al final)
void push(Stack *s, void *p);


// Quita un elemento de la pila
// Hacer un cast al tipo de datos de lo que quieras en la salida del pop, ya que voud * ke gusta a tidim oeri no al reves
void *pop(Stack *s); 


void freestack(Stack *s);