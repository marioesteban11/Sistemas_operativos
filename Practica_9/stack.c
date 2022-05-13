#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include "stack.h"


stack* create_stack(int max_size, int elemSize)
{
	
	stack *s = malloc(sizeof(stack));
	pthread_mutex_lock(&s->mutex);
	if (elemSize <=0){
		fprintf(stderr, "Numero de elementos no valido\n");
		return s;
	}
	//assert(elemSize > 0);
	s->elemSize = elemSize;
	s->logLength = 0;
	s->allocLength = max_size;
	s->elems = malloc(max_size * elemSize);
	if (s->elems == NULL)
	{
		fprintf(stderr, "Error al crear elems\n");
		return s;
	}
	//assert(s->elems != NULL);
	
	pthread_mutex_unlock(&s->mutex);
	return s;
}

void free_stack(stack *s)
{
	free(s->elems);
}


int is_empty(const stack *s)
{
	return s->logLength == 0;
}

int number_elements(stack *s){

	return s->logLength;
}

void push(stack *s, const void *elemAddr)
{
	pthread_mutex_lock(&s->mutex);
	void *destAddr;
	if(s->logLength == s->allocLength){
		s->allocLength *= 2;
		s->elems = realloc(s->elems, s->allocLength * s->elemSize);
		if (s->elems == NULL)
		{
			fprintf(stderr, "Error al modificar elems\n");
			return;
		}
		//assert(s->elems != NULL);
	}
	
	destAddr = (char *)s->elems + s->logLength * s->elemSize;
	memcpy(destAddr, elemAddr, s->elemSize);
	s->logLength++;
	pthread_mutex_unlock(&s->mutex);
}

void pop(stack *s, void *elemAddr)
{
	pthread_mutex_lock(&s->mutex);
	const void *sourceAddr;
	if (is_empty(s)){
		fprintf(stderr, "No se puede hacer pop de una stack vacia\n");
		return;
	}
	//assert(!is_empty(s));
	s->logLength--;
	sourceAddr = (const char *)s->elems + s->logLength * s->elemSize;
	memcpy(elemAddr, sourceAddr, s->elemSize);
	pthread_mutex_unlock(&s->mutex);
}