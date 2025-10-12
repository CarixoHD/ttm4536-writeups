#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>


void __attribute__ ((constructor)) disable_buffering()
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 1);
}

int win(){
    char buf[0x100] = {0}; 
    puts("You win! Here is your flag!:\n");
    int flag_fd = open("flag", O_RDONLY);
    
    if (flag_fd < 0){
        printf("\n ERROR: Failed to open the flag!");
        exit(-1);
    }
    
    int flag_len = read(flag_fd, buf, 0x100);
    
    if (flag_len <= 0){
        printf("\n ERROR: Failed to read the flag!");
        exit(-1);
    }

    write(1, buf, flag_len);
    puts("\n");
    return 1;
};

int main(int argc, char **argv, char **envp) { 
    char buf[112] = {0};
    long long n = 0;
    puts("-###-");
    printf(" ### Welcome to %s!\n", *argv);
    puts("-###-");

    puts("To get the flag in this challenge you will need to perform a buffer overflow attack");
    puts("Your input will be stored in a buffer that is 112 bytes long.");
    puts("The stack frame also contains a long long (8 bytes). Because of stack allignment there are an additional padding of 8 bytes in the stack frame.");
    puts("To overwrite the return address you must atleast send in 128 bytes. Remember to account for the base pointer stored just before the return address in the stack.");
    puts("Good luck!");
    
    printf("Please send in your data: ");
    gets(buf);
    printf("You entered %s\n", buf);
    return 1;
}
