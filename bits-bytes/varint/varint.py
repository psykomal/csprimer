import struct


def encode(n):
    out = []
    while n:
        part = n % 128
        n = n >> 7
        if n > 0:
            part = 128 + part
        out.append(part)
    return bytes(out)


def decode(varn):
    n = 0
    for b in reversed(varn):
        n <<= 7
        b &= 0x7F
        n |= b
    return int(n)


if __name__ == "__main__":
    with open("1.uint64", "rb") as f:
        n = struct.unpack(">Q", f.read())[0]
        print(n, encode(n), decode(encode(n)))
    with open("150.uint64", "rb") as f:
        n = struct.unpack(">Q", f.read())[0]
        print(n, encode(n), decode(encode(n)))
    with open("maxint.uint64", "rb") as f:
        n = struct.unpack(">Q", f.read())[0]
        print(n, encode(n), decode(encode(n)))
