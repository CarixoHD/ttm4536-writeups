#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

void __attribute__ ((constructor)) disable_buffering()
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 1);
}

void* shellcode; 
size_t shellcode_size;

int main(int argc, char **argv, char **envp) {
    
    for (int i = 3; i < 10000; i++) close(i);
    for (char **a = argv; *a != NULL; a++) memset(*a, 0, strlen(*a));
    for (char **a = envp; *a != NULL; a++) memset(*a, 0, strlen(*a));  
    
    puts("-###-");
    printf(" ### Welcome to shellcode 2!\n");
    puts("-###-");

    printf("This challenge will read in some bytes from you, and then execute them as code.\n");
    printf("In this challenge your shellcode must not be longer than 30 bytes!\n");

    shellcode = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANON, 0, 0);
    if (!shellcode){
        printf("ERROR: Memory allocation failed!\n");
        exit(-1);
    }
    printf("Reading in shellcode: ");
    shellcode_size = read(0, shellcode, 30);

    printf("You sent in %zu bytes!\n", shellcode_size);

    puts("Executing shellcode!\n");
    ((void(*)())shellcode)();

    printf("-###- Bye!\n");   

    return 1;
}
