
.SUFFIXES: .o .c
.c.o:
	$(CC) -c $(CFLAGS) $<


CC = gcc
CFLAGS = -g -c -Wall -Wshadow -O2 -pthread

SRC_PROXY = stack.h stack.c
SRC_1 = main.c $(SRC_PROXY)

OBJ1 = main.o stack.o


all: $(OBJ1)
	gcc ${CFLAGS} -o main $(OBJ1)

clean:
	$(RM) $(OBJ1) main
