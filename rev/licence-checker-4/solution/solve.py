from pwn import *
# alphabeth is from 32-126
licence_key = ">@-U'#01W\"jOG>6>9:2$"
xorkey = b"\x19\x1e\x52\x14\x02=@\x18\x06!\x0c\x13-\x1f"

vigener_key = "A+aMACkhqj{Ak\x16"

key = ""
for i in range(len(licence_key)):
    for j in range(95):
        v_key = vigener_key[i%(len(vigener_key))]
        if i == 13:
            v_key = vigener_key[12]
        if (j+(ord(v_key)-32))%96 == (ord(licence_key[i])-32):
            print(chr(j+32))
            key += chr(j+32)
print(key)
