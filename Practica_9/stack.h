

struct Valor{
  int v;
  int id;
};

struct Stack{
    int nelem;
    int max_elem;
    void *elementos;
};

typedef struct Stack Stack; 

int is_empty(Stack *s);
int nelem(Stack *s);
void push(Stack *s, void *p);
void *pop(Stack *s); 

Stack * newstack(int nelem);

void freestack(Stack *s);