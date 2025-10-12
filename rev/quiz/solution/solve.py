from pwn import *

p = process("./quiz")

p.recvuntil(b"64?\n")
p.sendline(b"1")
p.recvuntil(b"64?\n")
p.sendline(b"4")
p.recvuntil(b"start?\n")
p.sendline(b"4198784")
p.recvuntil(b"stack?\n")
p.sendline(b"1985161496")
p.recvuntil(b"register?\n")
p.sendline(b"57005")
p.recvuntil(b"stackframe?\n")
p.sendline(b"560")
p.interactive()
