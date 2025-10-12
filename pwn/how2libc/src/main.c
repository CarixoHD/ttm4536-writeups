#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void ignore_me_init_buffering() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}


void vuln4() {
    char buf[64];
    printf("\n\n------------------------------------------------------------------------\n\n");

    printf("Okay, now it's time to get shell!\n");
    printf("Use the libc leak, and your knowledge of ROP, to ROP inside libc to call system with /bin/sh as a parameter!\n");
    printf("Good luck!\n\n");
    printf("Enter your input: ");
    gets(buf);
    printf("\nReturning to: %p\n\n", __builtin_return_address(0));
}


void leak_got() {
    unsigned long input;
    unsigned long *inp_ptr;
    unsigned long addr;
    printf("But a problem is that the libc addresses are *almost* randomized. The last 12 bits are always the same while the rest are random!\n");
    printf("This means that if we leak a libc address, we can calculate the base address of libc, and then ROP using the offsets of the symbols/strings/gadgets we want!\n");
    printf("There are multiple ways to do this, but one way is to leak a libc address from the GOT!\n\n");
    printf("The GOT is a table that contains addresses of functions that are dynamically linked, meaning that the GOT contains pointers to libc functions!\n");
    printf("You can for example use readelf or gdb to find the GOT addresses!\n");
    printf("Now, give me a GOT address and I will leak it for you!\n\n");

    printf("Enter the address in hex (e.g. 0x7ffeef000): ");
    scanf("%lx", &input);
    getchar();
    printf("The address at 0x%lx is: 0x%lx\n", input, *(unsigned long *) input);

}

void vuln3(int a){
    if (a != 0xdeadbeef){
        printf("You didn't pass the correct argument!\n");
        exit(0);
    }
    printf("\n\n------------------------------------------------------------------------\n\n");
    printf("You successfully used a ROP gadget. Nice!\n");
    printf("These ROP gadgets are the building blocks of ROP chains!\n\n");
    printf("Until now, you have only used ROP gadgets from the binary file, however, like now, you don't have enough gadgets and stuff inside the binary file to get shell on the server.\n");
    printf("There is no system function/symbol nor a syscall gadget, but the libc file does contain the system function/symbol as well as a /bin/sh string!\n");
    printf("This means that we can ROP inside libc to call system with the parameters that we want (/bin/sh).\n");
    leak_got();
    
    printf("\n\n------------------------------------------------------------------------\n\n");

    printf("Now that we have a libc address (if the address you inputted was correct), we can calculate the base address of libc.\n");
    printf("The base address is the leaked libc address minus the offset of the leaked function/symbol. So let's say you leak the address of puts, and the offset of puts is 0x12345, then the base address is the leaked address minus 0x12345.\n");
    printf("If the calculated base address ends with 0x000 (since the last 12 bits are always the same), then you probably have the correct base address!\n\n");
    printf("Note: You don't always receive the libc file that the binary uses. ");
    printf("So sometimes you need to use a libc database and search for the offsets of the leaked functions/symbols to find libc versions that matches with the given offsets.\n");
    printf("(In this challenge you will receive the libc file that the binary uses, so you don't need to worry about that)\n");
    vuln4();
    printf("Looks like you failed to get shell :(\n");
    exit(0);


}

void vuln2(){
    char buf[64];
    printf("\n\n------------------------------------------------------------------------\n\n");

    printf("You successfully overwrote the return address! If this was a ret2win challenge, you would've gotten the flag!\n");
    printf("Now it's time to use a ROP gadget!\n\n");
    printf("Find and use a gadget that sets the first parameter of vuln3 to 0xdeadbeef, and then call the vuln3 function!\n");
    printf("Remember to use the correct gadget. You can check out the x64 calling convention to find out what register corresponds to what argument!\n");
    //printf("Set the first parameter of vuln3 to 0xdeadbeef and then call it!\n");
    printf("This time I am not giving you the address of vuln3. You can find it yourself using objdump, gdb, or readelf!\n\n");
    printf("Enter your input: ");
    gets(buf);
    printf("\nReturning to: %p\n", __builtin_return_address(0));


}

void vuln() {
    char buf[64];
    printf("You will now write to a buffer that can store 64 bytes (all the buffers in this challenge are 64 bytes)\n");
    printf("Let's overflow it and overwrite the return address so that it points to %p!\n\n", vuln2);
    printf("Also note that PIE is disabled, so all the binary addresses will be the same (not randomized) every time you run the program!\n");
    printf("Remember that you also need to overwrite the stored RBP that is located right before the return address!\n\n");
    
    printf("Enter your input: ");
    gets(buf);
    printf("\nReturning to: %p\n", __builtin_return_address(0));
}


void main() {
    ignore_me_init_buffering();

    printf("\t\t\t\t\t-----------------------------\n");
    printf("\t\t\t\t\t|                           |\n");
    printf("\t\t\t\t\t|    Welcome to ret2libc!   |\n");
    printf("\t\t\t\t\t|                           |\n");
    printf("\t\t\t\t\t-----------------------------\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("|\t");
    printf("**IMPORTANT!**\t\t\t\t\t\t\t\t\t\t\t\t\t|\n|\tPlease add a \"ret\" gadget as the first gadget in your ROP chains. This is because of stack alignment issues.\t|");
    printf("\n|\tSo, *padding* + ret gadget + symbol/some gadgets. If you don't add the \"ret\", the exploits will not work.       |\n");
    printf("-------------------------------------------------------------------------------------------------------------------------\n\n");

    printf("You will now be guided through the steps of how to solve an easy ret2libc challenge!\n");
    printf("Just a side note: pwntools can simplify everything in this challenge using the ROP class, but I recommend that you do it manually first!\n");
    printf("Another side note: when testing locally, YOUR libc will be used that might result in problems later. So you must patch your binary (how2libc) to use the libc file that is provided by the challenge (by using patchelf or pwninit), try to use the LD_PRELOAD environment, or test the later steps on remote.\nYou can solve this challenge without patching your binary.\n\n");
    printf("Also, here is a very nice resource you can use while solving this challenge: https://ir0nstone.gitbook.io/notes/types/stack/return-oriented-programming\n");
    printf("\n\n------------------------------------------------------------------------\n\n");
    vuln();
    printf("\nLooks like you failed to overwrite the return address :(\n");
    exit(0);
}