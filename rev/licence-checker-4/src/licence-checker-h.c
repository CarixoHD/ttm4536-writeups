#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stddef.h>
#include "b64/cdecode.h"
#include <sys/mman.h>

#define ALPHABET_SIZE 95
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

static char licenseKey[20] = ">@-U'#01W\"jOG>6>9:2$";
char *VIGENERKEY;
char *xorkey = "\x19\x1e\x52\x14\x02=@\x18\x06!\x0c\x13-\x1f";

char shellcode[] = "VUiJ5UiJffhIiXXwSItF8A+2AIPwWInCSItF+IgQSItF8EiDwAoPthBIi0X4SIPAAYPyJ4gQSItF8EiDwAUPthBIi0X4SIPAAoPyXIgQSItF8EiDwAEPthBIi0X4SIPAA4PyU4gQSItF8EiDwAcPthBIi0X4SIPABIPyWYgQSItF8EiDwAsPthBIi0X4SIPABYPyUIgQSItF8EiDwAQPthBIi0X4SIPABoPyaYgQSItF8EiDwAwPthBIi0X4SIPAB4PyRYgQSItF8EiDwAgPthBIi0X4SIPACIPyd4gQSItF8EiDwAMPthBIi0X4SIPACYPyfogQSItF8EiDwAYPthBIi0X4SIPACoPyO4gQSItF8EiDwA0PthBIi0X4SIPAC4PyXogQSItF8EiDwAkPthBIi0X4SIPADIPySogQSItF8EiDwAIPthBIi0X4SIPADYPyRIgQSItF+EiDwA7GAACQXcM=";

struct Vigener_entry{
    char character;
    int index_value;
};

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



int plusone(int i){
    int k = 1;
    i = i + k;
    return i;
}


int size(int j){
    int test = 0;
    int k = 0;
    while (j < 40){
        test = k + 1;
        j = plusone(j);
        k = plusone(k);
    }
    return test+56;
}

char *alphabet(int offset){
    int s;
    int k;
    char *buf = malloc(ALPHABET_SIZE);
    memset(buf, 0, ALPHABET_SIZE);
    k = 0;
    s = size(k);
    for (int i = 0; i < s; i++){
        buf[i] = ((i+offset) % s) + 32;
    }
    return buf;
}


void *transformation(char* inn, int si){
    struct Vigener_entry test;
    int vigener_entry_size = sizeof(test);
    void *struct_alph = malloc(si*sizeof(struct Vigener_entry));

    for (int i=0; i < si; i++){
        test.character = inn[i];
        test.index_value = inn[i]-32;
        memcpy((char *)struct_alph + (i*vigener_entry_size), &test, vigener_entry_size);
    }
    return struct_alph;
};


void *init_vigener(){
    int table_size;
    int struct_size;
    int si;
    struct_size =sizeof(struct Vigener_entry); 
    table_size = ALPHABET_SIZE*ALPHABET_SIZE*struct_size;
    si = ALPHABET_SIZE;
    void *vigener_table = malloc(table_size);
    
    for(int i = 0; i < 95; i++){
        char* m_table = alphabet(i);
        void* v_table = transformation(m_table, si);
        memcpy((char *)vigener_table +(i*(struct_size*ALPHABET_SIZE)), v_table, ALPHABET_SIZE*struct_size);
    }
    return vigener_table;
}

void encrypt(void *v_table, char input[], int struct_size, char *buf){
    char out[40] = {0};
    int curr_index = 0;
    int column;
    int row;
    for (int j = 0; j < 40; j++){
        
        for (int i = 0; i < ALPHABET_SIZE; i++){
            if ( *((char *)v_table+(i*struct_size)) == VIGENERKEY[j%strlen(VIGENERKEY)] ){
                column = *((int *)((char *)v_table+(i*struct_size)+offsetof(struct Vigener_entry, index_value)));
                break;
            }
        }

        for (int k = 0; k < ALPHABET_SIZE; k++){
            if ( *((char *)v_table+(k*struct_size)) == input[j] ){
                row = *((int *)((char *)v_table+(k*struct_size)+offsetof(struct Vigener_entry, index_value)));
                break;
            }
        }
        int offset = ((row * ALPHABET_SIZE) + column)*struct_size;  
        char encrypted_char = *((char *)v_table+offset);
        out[j] = encrypted_char;
    }
    memcpy(buf, out, 40);
    // printf("%s", out);
}

void* allocate_exec_mem(unsigned char* data, size_t len){    
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    size_t alloc_size = ((len+pagesize-1)/ pagesize) * pagesize;

    void* mem = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED){
        perror("mmap failed");
        return NULL;
    }
    memcpy(mem, data, len);
    return mem;
}


int init_vigener_key(){
    char decoded[1024] = {0};
    base64_decodestate decode_state;
    base64_init_decodestate(&decode_state);
    int dec_len = base64_decode_block(shellcode, strlen(shellcode), decoded, &decode_state);
    decoded[dec_len] = '\0';
    
    void* exec_mem = allocate_exec_mem(decoded, sizeof(decoded));
    if (!exec_mem) return 0;

    typedef void (*func_t)(char *key1, const char *key2);
    func_t func = (func_t)exec_mem;
    VIGENERKEY = malloc(15);
    func(VIGENERKEY, xorkey);
    return 1;
}

char *challenge(char input[]){
    int struct_size =sizeof(struct Vigener_entry); 
    char *out = malloc(40);
    void *v_table;
    memset(out, 0, 40);
    init_vigener_key();
    v_table = init_vigener();
    encrypt(v_table, input, struct_size, out);
    return out;
}

int main(int argc, char **argv, char **envp){
    char buf[40] = {0};
    int size = 40;

    puts("-###-");
    printf(" ### Welcome to %s!\n", *argv);
    puts("-###-\n");
    puts("This is a license verifier software!");
    puts("You must send in a correct license key over stdin that must be verified before you get access to the flag!");
    puts("This program may be doing some different operations on your input!");
    puts("To figure out what the license key must be, you have to reverse engineer this program.");
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
