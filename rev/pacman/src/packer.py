#!/usr/bin/env python3
import argparse, hashlib, os, random, subprocess, zlib
from jinja2 import Environment, FileSystemLoader
from pwn import ELF, context
import random

def render_payload_template(flag_check):
    env = Environment(loader=FileSystemLoader("templates"))
    tmpl = env.get_template("payload.c.j2")
    open("payload.c", "w").write(tmpl.render(FLAG_CHECK=flag_check))

def render_stub_template(xor_key, payload_size):
    env = Environment(loader=FileSystemLoader("templates"))
    tmpl = env.get_template("stub.asm.j2")
    open("stub.asm", "w").write(tmpl.render(XOR_KEY_PLACE=xor_key, PAYLOAD_SIZE_PLACE=payload_size))

def compile_payload():
    subprocess.check_call([
        "gcc", "-O2", "-fno-stack-protector", "-fPIE", "-pie", 
        "-fno-plt", "-o", "payload.bin", "payload.c"
    ])

def extract_shellcode():
    p = ELF("./payload.bin")
    main = p.sym["main"]
    text = p.get_section_by_name(".text")
    end = text.header.sh_addr + text.header.sh_size
    funcs = sorted(addr for addr in p.symbols.values() if isinstance(addr, int) and addr>main)
    main_end = funcs[0] if funcs else end
    return p.read(main, main_end-main), main_end-main

def encrypt_blob(shellcode, key):
    blob = bytes(b ^ key for b in shellcode)
    with open("encrypted_blob.bin","wb") as f: f.write(blob)

def assemble_stub():
    subprocess.check_call(["nasm","-fbin","-o","stub.bin","stub.asm"])

def inject_stub(target):
    elf = ELF(target)
    fini = elf.sym["_fini"]
    stub = open("stub.bin","rb").read()
    buf = open(target,"rb").read()
    with open(target,"wb") as f:
        f.write(buf[:fini] + stub + buf[fini+len(stub):])

def cleanup():
    for file in ["payload.c", "payload.bin", "encrypted_blob.bin", "stub.asm", "stub.bin"]:
        if os.path.exists(file):
            os.remove(file)

def main():
    binary = "pacman"
    #flag = f"flag{{{hashlib.md5(random.randbytes(32)).hexdigest()}}}"
    flag = f"flag{{{hashlib.md5(b"dusifhksdfhkdshfklhiuhk").hexdigest()}}}"

    print(f"[*] Flag: {flag}")
    checks = "\n    ".join(
        f'if ((uint8_t)input[{i}] != (uint8_t){ord(c)}) __do_syscall1(SYS_exit,1);'
        for i,c in enumerate(flag)
    )
    
    render_payload_template(checks)    

    compile_payload()
    shellcode, size = extract_shellcode()

    key = random.randrange(1,256)
    render_stub_template(key, size)

    encrypt_blob(shellcode, key)
    assemble_stub()

    inject_stub(binary)
    
    cleanup()
if __name__=="__main__":
    main()
