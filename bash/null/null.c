#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


//strrchr
int
main(int argc, char *argv[])
{    
    int fd;

    fd = open("/dev/null", O_WRONLY , 0664);

    if (fd == -1)
    {
        fprintf(stderr, "error");
    }
    return 0;
}