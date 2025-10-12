# Writeup licence-checker4

Author: `misi`

---
### Reversing

Start by doing `file licence-checker` to see what kind of file it is.

<img width="1557" height="35" alt="file" src="https://github.com/user-attachments/assets/0ca54f52-9ba1-488d-b41d-f9b4091b415c" />

It is a ELF executable that is dynamically linked, and it is stripped.

When i run the binary we can see that it is a similar challenge to licence-checker1, it asks for the users input, a licence-key, and then it prints the users input before it tells the user if it was correct or wrong.

<img width="758" height="235" alt="run" src="https://github.com/user-attachments/assets/ada78307-b09f-4db3-b819-6b38f9170a81" />

I use ghidra to decompile the binary

<img width="1919" height="747" alt="ghidra1" src="https://github.com/user-attachments/assets/ed294e10-4f38-4bbe-bd58-ba9d997295ef" />
  
From the entry function, the first argument to `__libc_start_main` is the `main` function.  

The main function prints some information, and then reads 0x28 bytes into `local_38`. It then calls a function with the address of your input as its argument. The return value of the function (`FUN_00101471`) is stored in `__src`, which is then copied into `local_38` before being freed. Finally, a memcmp between `local_38` and some data located in `.data`, comparing 0x14 (20) bytes.

<img width="907" height="768" alt="ghidra2" src="https://github.com/user-attachments/assets/59e18e1b-94a3-4ff5-9c94-f76e7389d0d4" />

If the memcmp returns 0, a function that opens and prints the flag will be called. If it does not, the program will exit.
Based on this behaviour and the information printed by main, we can deduce that FUN_00101a80 performs some transformation ont the input before it is compared to the licence key.  

<img width="347" height="282" alt="ghidra3" src="https://github.com/user-attachments/assets/c3b4af60-79ef-4721-b239-47810c595cc9" /> 

`FUN_00101a80` mallocs 0x28 (40) bytes and stores the address to the malloced memory in `__s`, then it calls `FUN_00101975`. After that it calls `FUN_00101651` and stores it's return value in uVar1. Finally it calls `FUN_001016ec` with uVar1, the user's input, 8, and `__s` as its arguments, before it returns `__s`.  

<img width="723" height="633" alt="ghidra4" src="https://github.com/user-attachments/assets/a6ae27d0-8948-4116-9ed4-55dad2ae6726" />

`FUN_00101975` does some interesting things, `FUN_00101cea` is a function that takes in a base64 string, its length, `local_418` and `local_420` as its arguments. 
`FUN_001018cb` takes `local_418` and 0x400 as arguments, and returns a code pointer, which is a pointer to some executable code, it is stored in pcVar3.
If `pcVar3` is not 0x0, it will execute it with 2 arguments, the first is a memory address pointing to some malloced memory (`DAT_001042f0`), and the other is a pointer to some bytes stored in memory.  
Finally the function returns.  

<img width="569" height="345" alt="ghidra5" src="https://github.com/user-attachments/assets/c8776c82-c064-4caf-a17a-2078baf9bf7a" />

`FUN_00101651` mallocs a big space of memory, 0x11a08 bytes. then it does a for loop where it calls 2 functions and a memcpy. First it calls `FUN_00101516` and then `FUN_00101596`, and finally it does a memcpy.  

<img width="644" height="320" alt="ghidra6" src="https://github.com/user-attachments/assets/d4178664-3f85-43d2-94cb-81a33da0e480" />

`FUN_00101516` takes `local_30`, the counter value for the for loop, in as its only argument. It mallocs 0x5f bytes of memory, and stores the pointer to the memory in `__s`. It stores the return value of `FUN_001014ca` in `iVar1` and then does a for loop that runs for `iVar1` times.  
In the for loop it stores a char in `__s`+`local_1c`. From this code we can see that it will fill __s with a string consisting of `iVar1` bytes. And then it will return the pointer to this string. 

<img width="320" height="337" alt="ghidra7" src="https://github.com/user-attachments/assets/e6886e37-e411-4fa2-a050-af165cc823fc" />

`FUN_001014ca`, takes in 0 as its argument, and will return 96.  

Now we now what `FUN_00101516` does. The function returns a string of 96 characters. The first character’s ASCII code is 32 + (param_1 % 96). Each subsequent character increases by 1 in ASCII value. In other words, the i-th character (starting at i = 0) is (char)((arg1 + i) % 96) +32.

<img width="545" height="444" alt="ghidra8" src="https://github.com/user-attachments/assets/de249c17-4b38-40f2-a2e4-d789c0a5a6f4" />

`FUN_00101596` takes the pointer to the string returned from `FUN_00101516` and 0x5f (95) as its arguments.  
It will malloc (95 << 3) = 760 bytes and store its pointer in `pvVar1`. It then does a for loop, where it will copy some part of the string (argument1) to `local_18[0]`. From this line we can deduce that `local_18` is some sort of struct. It will then copy a char from the argument1 string - 32, into `local_14`.  
Finally it copies 8 bytes from local_18 into the malloced memory that (`pvVar1` + `local_28`*8) points to.
After the for loop it will return the pointer `pvVar1`, which will now point to 760 bytes of memory that consists of data from the struct `local_18`, which also consists of data from the string argument.
This pointer is stored in `__src` in the `FUN_00101651` function.  

<img width="569" height="345" alt="ghidra5" src="https://github.com/user-attachments/assets/035265e6-f142-45ca-9d95-4344e20a2e78" />

Finally it copies the memory that `__src` points to into the larger malloced memory space that `pvVar1` points to. 
Overall we can deduce that the function will do a for loop 95 times, where each time it will create a 96 character long string, which is then stored in some struct, that is copied into a larger array of the struct, `__src` in the `FUN_00101651` function. Finally `__s` is stored in a bigger array, `pvVar1` in the `FUN_00101651` function, which will at the end of the function consist of 95 `__s` arrays.  
So it will return a large table, where each row will be an array of the `local_18` structs from the `FUN_00101596` function. And each column, will some `local_18` struct from the chosen row.  The `local_18` struct will hold a character from the string created in `FUN_00101516`.  
We can see this in gdb, if we break at the end of the `FUN_00101651` function. 

<img width="631" height="779" alt="gdb" src="https://github.com/user-attachments/assets/11bb251e-62ad-441f-a533-2b1f0053fcf7" />

The highlighted part, shows the first 95 local_18 structs that is stored in the malloced memory, each struct is 8 bytes and consists of a ascii character and an int.  

We can assume that `FUN_001016ec` is the function that will do some mangling to our input, as it is the last function called in the original function that is deduced to mangle our input. It also takes in our input as one of its arguments. 

**WILL BE UPDATED**  






