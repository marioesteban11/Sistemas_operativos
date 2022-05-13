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
	int v;
	int id;
}valor;

enum {
	Nthreads = 100,
};

pthread_mutex_t mutex;
valor stack_valor; 
stack *intStack;


void *fn(void *p)
{
	
	int id;
	valor salida;

	
	id = *(int*)p;
	salida.id = id;
	//fprintf(stderr, "HILO :%d\n", stack_valor.id);
	
	for(int val = 0; val < 100; val++){
		salida.v = val;
		push(intStack, &salida);
	}
	
	for(int i = 40; i > 0; i--){
		pop(intStack, &salida);
		if (salida.id != id){
			printf("Poped value: %d del hilo: %d y seria del valor: %d\n", salida.id, salida.v, id);
		}
	}
	return NULL;
}

int
main(int argc, char *argv[])
{	
	pthread_t thread[Nthreads];
	int n_threads[Nthreads];
	int i;
	int counter = 0;
	int max_size = 50;
	intStack = create_stack( max_size, sizeof(intStack));
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
	if (counter == 60*100){
		printf("Buen resultado\n");
	}else {
		printf("Mal resultado. EL resultado es %d y tendría que ser %d\n", counter, 60*100);
	}
   	
	free_stack(intStack);
	exit(EXIT_SUCCESS);
}
