#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>


void payload_main(int argc, char **argv, char **envp, void *arg) {
    struct {
        int (*printf)(const char *, ...);
        void *(*malloc)(size_t);
        char* (*strcpy)(char* destination, const char* source);
        int (*scanf)(const char *, ...);
        void (*exit)(int);
    } *libc = arg;
    int ans;
    long long answer;
    long long check;
    char scan[] = 
        "%d\0"
        "%lld\0";
    char questions[] = 
        "What is the size of a char in bytes on x86_64?\n\0"
        "What is the size of an int in bytes on x86_64?\n\0"
        "What did I just push onto the stack?\n\0"
        "What is the current value of the rbx register?\n\0"
        "How big is the current stackframe?\n\0"
        "Where does .text start?\n\0";
    char tekst[] =
        "-###-\n\0"
        " ### Welcome to %s!\n\0"
        "\n\0"
        "This is a quiz! Answer all the questions correctly to win!\n\0"
        "Correct!\n\0"
        "WRONG!\n\0";

    char *output[] = {
        &tekst[0],
        &tekst[7],
        &tekst[28],
        &tekst[30],
        &tekst[90],
        &tekst[100],
        &questions[0],
        &questions[48],
        &questions[96],
        &questions[134],
        &questions[182],
        &questions[218],
    };
    libc->printf(output[0]);
    libc->printf(output[1], *argv);
    libc->printf(output[0]);
    libc->printf(output[2]);
    libc->printf(output[3]);
    libc->printf(output[6]);
    libc->scanf(&scan[0], &ans);
    if(ans != 1){
        libc->printf(output[5]);
        libc->exit(-1);
    }
    libc->printf(output[4]);
    libc->printf(output[7]);
    libc->scanf(&scan[0], &ans);
    
    if(ans != 4){
        libc->printf(output[5]);
        libc->exit(-1);
    }
    libc->printf(output[4]);
    libc->printf(output[11]);
    libc->scanf(&scan[3], &answer);
    
    if (answer != 0x401180){
        libc->printf(output[5]);
        libc->exit(-1);
    }

    libc->printf(output[4]);
    __asm__ (
    ".intel_syntax noprefix;\n"
    "push 0xdead;\n"
    "push 0x76532918;\n"
    ".att_syntax;"
    );
    libc->printf(output[8]);
    libc->scanf(&scan[3], &answer);
    __asm__ (
    ".intel_syntax noprefix;\n"
    "pop rcx;\n"
    "pop rbx;\n"
    "mov %[out], rcx;\n"
    ".att_syntax;"
    : [out] "=r" (check)
    :
    : "rcx"
    );
    if(answer != check){
        libc->printf(output[5]);
        libc->exit(-1);
    }
    
    libc->printf(output[4]);
    libc->printf(output[9]);
    
    libc->scanf(&scan[3], &answer);
    __asm__ (
    ".intel_syntax noprefix;\n"
    "mov %[out], rbx;\n"
    ".att_syntax;"
    : [out] "=r" (check)
    :
    : "rbx"
    );
    if(answer != check){
        libc->printf(output[5]);
        libc->exit(-1);
    }
    libc->printf(output[4]);
    libc->printf(output[10]);
    libc->scanf(&scan[0], &ans);

    __asm__ (
    ".intel_syntax noprefix;\n"
    "mov rcx, rbp;\n"
    "sub rcx, rsp;\n"
    "mov %[out], rcx;\n"
    ".att_syntax;"
    : [out] "=r" (check)
    :
    : "rcx"
    );
    if (ans != check){
        libc->printf(output[5]);
        libc->exit(-1);
    }
    libc->printf(output[4]);
}

