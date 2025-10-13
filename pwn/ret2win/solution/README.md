# Ret2win

Author: `misi`

---
### Reversing
In this challenge we are given a binary file. To reverse this I will use ghidra.  
First I will do checksec on the binary to see which security measures is enabled.

<img width="570" height="142" alt="ret2win1" src="https://github.com/user-attachments/assets/2d0331bc-4122-4718-a842-2b3c61a68cc4" />

There is no stack canary, Pie is not enabled and the file is not stripped.  

<img width="1283" height="534" alt="ret2win2" src="https://github.com/user-attachments/assets/e4ce14c1-9326-45e0-b692-2fac796fe4ba" />

From the symbol tree in ghidra we can see there are 2 functions we need to focus on: win() and main().  

<img width="777" height="587" alt="ret2win0" src="https://github.com/user-attachments/assets/4274c92b-2485-400b-8e64-abf1c41e4eed" />

The main function will first print out alot of information and will then take your input with the gets function.  
gets is a known vulnerable function since it will read in you input and there is no limit in how many bytes you can send in. 
The program will print out what you entered and will then return.

The win function will open the flag, and then print it to stdout.  

### Exploitation
The goal for this challenge is to overwrite the return adress with the address to the win function.  
To do this we must create a payload to send to the challenge.  

The main function will print out how large the buffer is and how many bytes you need to send in to do a buffer overflow.  
From this we know that we need to fill the buffer with atleast 128 bytes.  

Because the base pointer is stored on the stack after the return address, we will need to overwrite it too, increasing the size of the payload to 136 bytes.  

Finally we need to overwrite the return address with the address to win().  
There are many ways to get the address to win(). Since PIE is not enabled you can just read the address from ghidra.  

<img width="770" height="192" alt="ret2win3" src="https://github.com/user-attachments/assets/5b8b49a9-beec-47ff-871f-3dde303b9d55" />

The payload will consist of 136 bytes and the the win() address we found from ghidra. 

When we provide the payload to the vulnerable gets function, it will overflow the buffer and overwrites the return address on the stack with the address of win(). When the main function returns, it will return directly to win(), which then executes and prints the flag to stdout.

Final solvescript:  

```py 
from pwn import *

context.arch = "amd64"
context.terminal = ['tmux', 'splitw', '-h']
context.binary = "./ret2win"

win = 0x4011d9

p = process("./ret2win")

p.recvuntil(b"data:")
p.sendline(b"A"*128 + b"A"*8 + p64(win))

p.interactive()
```
