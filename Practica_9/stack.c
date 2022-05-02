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
	pthread_mutex_lock(&create);
	stack *s = malloc(sizeof(stack));
	assert(elemSize > 0);
	s->elemSize = elemSize;
	s->logLength = 0;
	s->allocLength = max_size;
	s->elems = malloc(max_size * elemSize);
	assert(s->elems != NULL);
	//fprintf(stderr, "STACK CREADA CON n_elementos: %d y esta vacía: %d", s->allocLength, is_empty(s));
	pthread_mutex_unlock(&create);
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
	pthread_mutex_lock(&push_stack);
	void *destAddr;
	if(s->logLength == s->allocLength){
		s->allocLength *= 2;
		s->elems = realloc(s->elems, s->allocLength * s->elemSize);
		assert(s->elems != NULL);
	}
	
	destAddr = (char *)s->elems + s->logLength * s->elemSize;
	memcpy(destAddr, elemAddr, s->elemSize);
	s->logLength++;
	pthread_mutex_unlock(&push_stack);
}

void pop(stack *s, void *elemAddr)
{
	pthread_mutex_lock(&pop_stack);
	const void *sourceAddr;
	assert(!is_empty(s));
	int real = s->logLength;
	int funcion = number_elements(s);
	printf("log_lenght en funcion: %d y en realidad: %d\n", funcion, real);
	s->logLength--;
	sourceAddr = (const char *)s->elems + s->logLength * s->elemSize;
	memcpy(elemAddr, sourceAddr, s->elemSize);
	pthread_mutex_unlock(&pop_stack);
}



//void recorrer_stack(stack *s)
//{
//	for (int i = 0; i < s->logLength; i++){
//		fprintf(stderr, "%d \t", s->elems[i]);
//	}
//
//}