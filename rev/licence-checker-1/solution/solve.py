from pwn import *

key = b"aypuv-Apm4Y-Lf1l8-W0fz6-yM8JC"

# p = process("./licence-checker")
p = remote("localhost", 2727)

p.recvuntil(b"key: ")
p.send(key+b"\x00")

p.interactive()

