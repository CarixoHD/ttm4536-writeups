# Writeup quiz

Author: `misi`

---
### Reversing
Start by doing `file quiz` to see what kind of file it is.

<img width="1453" height="33" alt="file" src="https://github.com/user-attachments/assets/d7973ae0-425b-48a2-aa3f-b23b994d417d" />

It is a stripped ELF executable that is dynamically linked.

First I try to execute the file:

<img width="440" height="241" alt="run_quiz" src="https://github.com/user-attachments/assets/cbc4e8ea-a6d3-4dea-a2df-3c2673f5ed18" />

It tells us that it is a quiz program and starts asking trivia questions.

I think that it should be possible to find out the answers easily, by reversing the binary.

I use ghidra to decompile it:

<img width="1907" height="752" alt="entry_ghidra" src="https://github.com/user-attachments/assets/42af0aa0-66c5-4ec4-94bd-4db2fb0d252a" />

I go to the main function that is called by __libc_start_main in the entry function.

<img width="1919" height="1033" alt="main_ghidra" src="https://github.com/user-attachments/assets/cd6b8e08-24fb-42a1-8a5b-0dc158ab409e" />

I can see that some memory is malloced, and then the pointer to the memory used as an argument for FUN_004012c1:

<img width="821" height="808" alt="Decryption_ghidra" src="https://github.com/user-attachments/assets/521e2fbe-17c7-4e74-bff6-3a0bfd2df4a0" />

Here we can see that there is some decryption happening.

<img width="637" height="404" alt="fun_4013b5_ghidra" src="https://github.com/user-attachments/assets/c2cb9f8b-8349-4bc9-b23c-8747bf3798ac" />

<img width="516" height="270" alt="Interesting_ghidra" src="https://github.com/user-attachments/assets/7c77e7da-e89c-4a34-b3e5-16faba7a1ed4" />

FUN_004013b5 returns a code pointer that is stored in pcVar2, pcVar2 is then run if it is not 0x0.

<img width="472" height="847" alt="win_ghidra" src="https://github.com/user-attachments/assets/d7b851af-0644-4705-b1ee-eef316f1cf5e" />

FUN_0040144f is executed after pcVar2, and it looks like it is a function that opens a "flag" file and then outputs its content to stdout.

We dont know how the code in pcVar2 looks like, but we can assume that it is the code that actually prints the quiz questions and takes in the users input.  
It is possible to decompile the code in pcVar2 by decrypting the value in memory, but for this writeup I will instead do a dynamic analysis of the program to figure out what is happening in pcVar2.

### Solution

There are multiple ways to solve the challenge, in this writeup I will solve it by using pwndbg to debug the program during runtime, to see what answers each question expects.

run `gdb ./quiz` to start pwndbg. To start the program and break at the entry function I use the `start` command.

<img width="1164" height="802" alt="gdb1" src="https://github.com/user-attachments/assets/689c1058-742c-4f31-a124-5f615c2d9879" />

<img width="554" height="107" alt="checksec" src="https://github.com/user-attachments/assets/f30ae18c-6f03-45d6-a934-1d6af1729817" />

Since there is no PIE, I find the address to the instruction which calls the pcVar2 function in Ghidra

<img width="1280" height="329" alt="ghidra2" src="https://github.com/user-attachments/assets/ae5a2778-b5c1-43b0-af71-49b7ebd77b1e" />

It is 0x004017ed
I set a breakpoint for this address in pwndbg with: `b *0x004017ed`, then continue the program until it hits the breakpoint, by using `c`.  

<img width="1191" height="881" alt="gdb2" src="https://github.com/user-attachments/assets/1c9bd71a-46e2-455a-ab59-fddef982e456" />

Now I step into the function, using `si`, so that I can go through the function with pwndbg.  
I keep stepping through the function until I reach the first scanf function call.  

<img width="1029" height="818" alt="gdb3" src="https://github.com/user-attachments/assets/92803240-61aa-4913-9764-bc512dee040f" />
 
Here we can see that the input to scanf is stored in the address in RSI, so `0x7fffffffd7a4`. We also see that after the scanf function call, some value stored in RBP-0x20c is copied into eax, which is then compared to 1.  
By checking what value RBP-0x20c is, we can see that it is the same value as `0x7fffffffd7a4`. Which means that our input is compared against 1.  
I send in 1 as input.

I keep stepping through the function, and can see that the first printf prints `Correct!\n`. I keep stepping through the function until I reach the second scanf.  

<img width="1168" height="851" alt="gdb4" src="https://github.com/user-attachments/assets/1d6bb383-5224-4974-8087-6555348db15d" />
  
Here we can see that our input is compared against 4, so I send in 4 as input to scanf. Again I can see that this was the correct input.  
Again I step through the function until I reach a scanf.  

<img width="1156" height="857" alt="gdb5" src="https://github.com/user-attachments/assets/a9b2c63f-1997-4536-809b-d95d708436ac" />

Here we see that out input is compared against `0x401180`. Since scanf is expecting a long long integer value in decimal format, (since it uses `%lld` as its first argument), we need to convert `0x401180` to decimal format before we send it in.  
We can achieve this by using this command in pwndbg: `p 0x401180`, which results in this output: `$1 = 4198784`.  

<img width="161" height="34" alt="gdb6" src="https://github.com/user-attachments/assets/eabfa371-1553-4830-911b-e4d3ebf9a2b4" />  

I send in this value to scanf. And again I continue stepping through the function until I reach the next scanf.  
Here we can use `x/10i $rip` to see the next 10 instructions.

<img width="1147" height="940" alt="gdb7" src="https://github.com/user-attachments/assets/665a0e35-e263-455d-8305-10ee4408e32b" />

We can see here that the top of the stack is popped into rcx, which is copied into rax, which is then copied into rbp-0x1f8.  
This value is then compared to our input, so to see what we should send in we just need to look what is in the top of the stack.  
We do this by using the `stack` command.  

<img width="963" height="150" alt="gdb8" src="https://github.com/user-attachments/assets/549b41eb-9695-4a59-bc03-ad734a09e94d" />

Here we can see the top of the stack contains the value `0x76532918`, which is `1985161496` in decimal format. I send this value in to scanf.  
I continue stepping through until I reach the next scanf function.  

<img width="1079" height="841" alt="gdb9" src="https://github.com/user-attachments/assets/6dbfecdf-a4d5-4447-8f05-e56c3b9bf738" />

Here we can see that our input is compared against the value stored in RBX. We can see that `0xdead` is stored in RBX, which is `57005` in decimal.

<img width="1027" height="939" alt="gdb10" src="https://github.com/user-attachments/assets/93835206-b169-4804-a43e-6b30fea3cc2b" />

For the final question, we can see that our input is compared against the result of RBP-RSP. We can find this value by finding the values of RBP and RSP and then substracting RBP with RSP. Which is the same as `560`.  

<img width="417" height="405" alt="gdb11" src="https://github.com/user-attachments/assets/36c2c094-803e-45f1-b5a6-1a8c5cd37008" />

<img width="313" height="32" alt="gdb12" src="https://github.com/user-attachments/assets/6b1831d5-36d2-4f21-af0b-a01f72e2bd75" />

I send in 560 and then I continue the program wth pwndbg. I can see that the FUN_0040144f function is called, but since im doing this locally I dont have the actual flag file.  

I create a python script that will send in all of the answers I have found to the remote server.
This is my final solve script:  

```py
from pwn import *

p = process("./quiz")

p.recvuntil(b"64?\n")
p.sendline(b"1")
p.recvuntil(b"64?\n")
p.sendline(b"4")
p.recvuntil(b"start?\n")
p.sendline(b"4198784")
p.recvuntil(b"stack?\n")
p.sendline(b"1985161496")
p.recvuntil(b"register?\n")
p.sendline(b"57005")
p.recvuntil(b"stackframe?\n")
p.sendline(b"560")
p.interactive()
```






