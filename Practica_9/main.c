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
valor stack_valor; 
stack *intStack;


void *fn(void *p)
{
	int max_size = 50;
	int id;
	
	//valor *stack_valor; 
	id = *(int*)p;
	intStack = create_stack( max_size, sizeof(int));
	stack_valor.id = id;
	//fprintf(stderr, "HILO :%d\n", stack_valor.id);
	
	for(int val = 0; val < 10; val++){
		push(intStack, &val);
	}
	
	for(int i = 4; i >= 0; i--){
		pop(intStack, &i);
		
		if (stack_valor.id != id){
			fprintf(stderr, "Poped value: %d del hilo %d, y el stack_valor: %d\n", i, id, stack_valor.id);
		}
	}

	//free_stack(intStack);
	
	return NULL;
}

int
main(int argc, char *argv[])
{	
	pthread_t thread[Nthreads];
	int n_threads[Nthreads];
	int i;
	int counter = 0;
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

	for (int j = 0; j < intStack->logLength;j++ ){
		counter++;
	}
	if (counter == 6*10){
		printf("Buen resultado\n");
	}else {
		printf("Mal resultado. EL resultado es %d y tendría que ser %d\n", counter, 60*100);
	}
   	
	exit(EXIT_SUCCESS);
}
