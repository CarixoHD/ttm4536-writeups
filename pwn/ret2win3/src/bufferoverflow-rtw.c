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

static const char msg_open_fail[] = "\n ERROR: Failed to open the flag!\n";
static const char msg_read_fail[] = "\n ERROR: Failed to read the flag!\n";
static const char newline[] = "\n";

int win(int rdi){
    puts("You win! Here is your flag!:\n");
    if (rdi == 0x1337){
        __asm__(
                ".intel_syntax noprefix;\n"
                "sub rsp, 0x100;\n"
                "mov rax, 2;\n"
                "mov rdi, 0x67616c66;\n"
                "push rdi;\n"
                "mov rdi, rsp;\n"
                "mov rsi, 0;\n"
                "mov rdx, 0;\n"
                "syscall;\n"
                "cmp rax, 0;\n"
                "jg no_neg;\n"
                "lea rdi, [rip + msg_open_fail];\n"
                "call puts; \n"
                "mov rax, -1;\n"
                "call exit;\n"
                "no_neg:\n"
                "mov rdi, rax;\n"
                "mov rax, 0;\n"
                "pop rbx;\n"
                "mov rsi, rsp;\n"
                "mov rdx, 0x100;\n"
                "syscall;\n"
                "cmp rax, 0;\n"
                "jg no_nega;\n"
                "lea rdi, [rip + msg_read_fail];\n"
                "call puts;\n"
                "mov rax, -1;\n"
                "call exit;\n"
                "no_nega:\n"
                "mov rdx, rax;\n"
                "mov rdi, 1;\n"
                "mov rax, 1;\n"
                "syscall;\n"
                "lea rdi, [rip+newline];\n"
                "call puts;\n"
                ".att_syntax;"
                :
                :
                : "rax", "rdi", "rbx", "rsi", "rdx", "memory"
                );
    }
    else {
        puts("Oh, you are no leet hacker...");
        exit(-1);
    }
    return 1;
};

int main(int argc, char **argv, char **envp) { 
    char buf[72] = {0};
    long long n = 0;
    int c = 0;
    puts("-###-");
    printf(" ### Welcome to %s!\n", *argv);
    puts("-###-");

    puts("To get the flag in this challenge you will need to perform buffer overflow attack");
    puts("Your input will be stored in a buffer that is 72 bytes long.");
    puts("The stack frame also contains a long long (8 bytes), and an int (4 bytes).");
    puts("Due to stack allignment, there are an additional 12 bytes of padding in the stack frame.");
    puts("To overwrite the return address you must atleast send in 96 bytes, not including the base pointer.");
    puts("Good luck!");
    
    printf("Please send in your data: ");
    gets(buf);
    printf("You entered %s\n", buf);
    return 1;
}
