from libdebug import debugger
from capstone import *


def fini_callback(t, bp):
    flag = ""
    instr = d.memory[t.regs.rip:t.regs.rip+0x2000]
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    for i in md.disasm(instr, t.regs.rip):
        if i.mnemonic == "cmp":
            flag += chr(i.bytes[-1])
    print("FLAG:", flag)



d = debugger("./pacman", auto_interrupt_on_command=True)
p = d.run()

# catch "open" syscall
d.handle_syscall(2, fini_callback)

d.cont()

p.interactive()

