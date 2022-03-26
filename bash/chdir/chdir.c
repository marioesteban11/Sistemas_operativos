#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/stat.h>

enum{
    SIZE_DIR = 100,
};


void 
change_directory(char * directorio){
    int error;
    char s[SIZE_DIR];
  
    // printing current working directory
    printf("%s\n", getcwd(s, SIZE_DIR));
  
    // using the command
    error = chdir(directorio);
  
    if (error != 0 ){
        perror("chdir failed");
    }

    // printing current working directory
    printf("%s\n", getcwd(s, SIZE_DIR));
}

int
main(int argc, char *argv[])
{
    
    change_directory(argv[1]);
    // after chdir is executed
    return 0;

}