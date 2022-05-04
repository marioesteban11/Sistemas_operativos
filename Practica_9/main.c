#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "stack.h"

typedef struct{
	int i;
	int id;
}valor;

enum {
	Nthreads = 10,
};

pthread_mutex_t mutex;

void *fn(void *p)
{
	
	int max_size = 50;
	int id;
	stack *intStack;
	//valor *stack_valor; 
	id = *(int*)p;
	intStack = create_stack( max_size, sizeof(int));
	//fprintf(stderr, "HILO :%d\n", id);
	//stack_valor->id = id;
	for(int val = 0; val < 10; val++){
		push(intStack, &val);
	}

	for(int i = 5; i >= 0; i--){
		pop(intStack, &i);
		printf("Value Poped: %d from thread: %d\n", i, id);
	}

	pthread_mutex_lock(&push_stack);
	printf("Elementos restantes: %d, del hilo: %d\n", number_elements(intStack), id);
	pthread_mutex_unlock(&push_stack);
	free_stack(intStack);
	
	return NULL;
}

int
main(int argc, char *argv[])
{	
	pthread_t thread[Nthreads];
	int n_threads[Nthreads];
	int i;
	for(i = 0; i < Nthreads; i++){
		n_threads[i] = i;
		if(pthread_create(&thread[i], NULL, fn, (void *) &n_threads[i])) {
			warn("error creating thread");
			return 1;
		}
	}

	for(i=0; i<Nthreads; i++){
		if(pthread_join(thread[i], NULL) != 0){
			warn("error joining thread");
			return 1;
		}
	}

   	
	exit(EXIT_SUCCESS);
}
