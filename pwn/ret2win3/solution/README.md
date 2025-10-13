# Ret2win3

Author: `misi`

---
### Reversing
First I do a checksec on the given binary to figure out the security features enabled.  

<img width="573" height="136" alt="ret2win_3" src="https://github.com/user-attachments/assets/dce55970-1792-4078-bcb9-a1cc42080a69" />
  
We can see that: 
- Stack canary is not enabled
- Pie is not enabled
- The binary is not stripped

I will use ghidra to decompile the binary.  
The most important functions to focus on is main() and win().  

<img width="847" height="626" alt="ret2win1_3" src="https://github.com/user-attachments/assets/6a1c900f-a334-4c8f-a9c3-b1acd3d85741" />

The main function will print out alot of information, call gets to take in your input, print out your input the return.  

<img width="645" height="522" alt="ret2win2_3" src="https://github.com/user-attachments/assets/64770d7c-491f-4279-9dc6-03f786a27bd8" />

The win function will check if the first parameter is equal to 0x1337, after that it will do some syscalls before it exits.  
By reversing the assembly code for win() we can see what syscalls are being called.  

<img width="875" height="506" alt="ret2win3_3" src="https://github.com/user-attachments/assets/6922906b-4878-4d41-a0fe-5ad584d19c57" />

After the if comparison, the program will do the open syscall on "flag". If it fails it will print something out and then exit.  

<img width="915" height="618" alt="ret2win4_3" src="https://github.com/user-attachments/assets/d4e883db-6d19-4cb6-b09f-1cc4fa8867c5" />

If it suceeds it will do a read of the flag file then it will write what it read to stdout (as long as it does not fail to read).  
Finally it returns.  

### Exploitation  
The goal of this challenge is to do a buffer overflow and overwrite the return address to an address in win().  
We cant return directly to the beginning of win() since we cant change rdi to 0x1337.  

As the main() function will print out how big the buffer is (96 bytes), we know that the payload must be 96+8=104 bytes long to be able to overwrite the return address.  
Now we must find an address in win() which will skip the if check on rdi, but not crash the process of opening and reading the flag.  
Since PIE is not enabled we can read the address we want to use from ghidra.

<img width="548" height="250" alt="ret2win5_3" src="https://github.com/user-attachments/assets/72072af4-0a66-4195-b928-7cc30c8a3f72" />

When we provide the payload to the vulnerable gets function, it will overflow the buffer and overwrites the return address on the stack with the address we want to use in win(). When the main function returns, it will return directly to the adress we provided, which then executes and prints the flag to stdout.

This is the final solve script:
```py
from pwn import *

p = process("./ret2win")

win = 0x4011d1
p.recvuntil(b"data: ")
p.sendline(b"A"*104+p64(win))
p.interactive()
```
