import sys


def trunc(s, n):
    if n >= len(s):
        return s

    while n > 0 and ((s[n] & 0xC0) == 0x80):
        n -= 1
    print(n)
    return s[:n]


with open("cases", "rb") as f:
    for line in f.readlines():
        n = line[0]
        s = line[1:-1]
        print(n, s, len(s))
        wrong_s = s[:n]
        sys.stdout.buffer.write(wrong_s + b"\n")
        s = trunc(s, n)
        sys.stdout.buffer.write(s + b"\n")
        print("\n\n")
