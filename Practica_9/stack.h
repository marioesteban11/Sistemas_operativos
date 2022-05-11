typedef struct{
    void *elems;
    int elemSize;
    int logLength;
    int allocLength;
    pthread_mutex_t mutex;
}stack;

stack* create_stack( int max_size, int elemSize);
void free_stack(stack *s);
int is_empty(const stack *s);
void push(stack *s, const void *elemAddr);
void pop(stack *s, void *elemAddr);
int number_elements(stack *s);

void recorrer_stack(stack *s);