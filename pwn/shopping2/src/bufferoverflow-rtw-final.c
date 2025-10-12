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

struct shopping_list
{
    int recv;
    char list[235]; 
    unsigned long size;
};


int win(){
    char buf[0x100]; 
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


int menu(){
    int choice;
    int c;
    printf("\n----- MENU -----\n");
    printf("Option 1: Write to shopping list\n");
    printf("Option 2: Read from shopping list\n");
    printf("Option 3: Exit\n");
    printf("\nEnter a choice (1-3): ");
    if (scanf("%d", &choice) != 1){
        printf("Invalid input!\n");
        while ((c = getchar()) != '\n' && c != EOF);
        if (c == EOF){
            exit(0);
        }
        return -1;
    }
    if (choice > 3 || choice < 1){
        printf("Invalid input!\n");
        return -1;
    }
    printf("You chose: %d\n", choice);
    if (choice == 1){
        return 1;
    }
    if (choice == 2){
        return 2;
    }
    if (choice == 3){
        return 3;
    }
    return -1;
}


int challenge(int argc, char **argv, char **envp){
    int choice = 0;
    struct shopping_list shop_list = {0};
    while (1) {
        choice = menu();
        if (choice == -1){
            continue;
        }
        if (choice == 1){
            printf("Please send in how many bytes you wish to write to the shopping list: ");
            scanf("%lu", &shop_list.size);
            printf("\nYou hace chosen to send %lu bytes of input!\n", shop_list.size);
            printf("Please send in your data:\n");
            shop_list.recv = read(0, shop_list.list, (unsigned long) shop_list.size);
            if (shop_list.recv < 0){
                printf("ERROR: failed to read input\n");
                continue;
            }
            printf("You sent %d bytes!\n", shop_list.recv);
        }
        if (choice == 2){
            printf("This is the current shopping list:\n");
            printf("%s\n", shop_list.list);
            continue;
        }
        if (choice == 3){
            printf("You have chosen to exit the program\n");
            printf("Goodbye!\n");
            return 1;
        }
    }
    return 1;
}

int main(int argc, char **argv, char **envp) { 
    char buffer[1000];
    puts("-###-");
    printf(" ### Welcome to %s!\n", *argv);
    puts("-###-\n");
    challenge(argc, argv, envp);
    return 1;
}
