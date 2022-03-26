#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/stat.h>

enum {
    MAX_STDIN = 100,
};


int
main(int argc, char *argv[])
{


    while(1)
    {
        char buf[MAX_STDIN];
        fgets(buf, MAX_STDIN, stdin);
        printf("string is: %s\n", buf);
    }

    



    return 0;
}