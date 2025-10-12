#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>

char licenseKey[30] = "HZbpB-JOJIz-fvEjR-V5wR3-oHcbf";

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
}

char *challenge(char input[]){
    char *out = malloc(40);
    memset(out, 0, 40);
    for (int i = 0; i < 40; i++){
        out[i] = input[i];
    }
    return out;
}

int main(int argc, char **argv, char **envp){
    char buf[40] = {0};
    int size = 40;
    puts("-###-");
    printf(" ### Welcome to %s!\n", *argv);
    puts("-###-\n");
    puts("This is a license verification program!");
    puts("You must provide a valid license key via stdin, which will be verified before granting access to the flag!");
    puts("This program may perform various operations on your input!");
    puts("To figure out the correct license key, you'll need to reverse engineer this program.");
    puts("Ready to receive your license key: ");
    read(0, buf, size);
    printf("Input before mangling: %s\n", buf);
    char *out = challenge(buf);
    memcpy(buf, out, size);
    free(out);
    if ( !memcmp(buf, licenseKey, sizeof(licenseKey)) )
    {
        win();
        exit(0);
    }
    puts("Wrong! No flag for you!");
    exit(1);
    return 0;
}
