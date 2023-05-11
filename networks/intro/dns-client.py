import sys
import socket
import struct
import random

"""
    DNS RFC - https://www.ietf.org/rfc/rfc1035.txt
"""

GOOGLE_PUBLIC_DNS = ("8.8.8.8", 53)

def skip_bytes(res, i):
    
    while True:
        x = res[i]
        if x & 0b11000000 == 0b11000000:
            return i + 2
        if x == 0x00:
            i += 1
            break
        i += x + 1
    
    return i


if __name__ == "__main__":
    hostname = sys.argv[1]
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    xid = random.randint(1, 0xffff)
    flags = 0x0100  # 0 0000 0010 000 0000
    query = struct.pack("!HHHHHH", xid, flags, 1, 0, 0, 0)
    qname = b"".join(
                (len(p).to_bytes(1, "big") + p.encode("ascii")) 
                for p in hostname.split(".")
            ) + b"\x00"
    query += qname
    query += struct.pack("!HH", 1, 1)
    
    s.sendto(query, ("8.8.8.8", 53))
    
    check = 0
    while check != 2:
        print('this')
        res, sender = s.recvfrom(4096)
        if sender != GOOGLE_PUBLIC_DNS:
            continue
        else:
            check += 1
    
        rxid, flags, qd_count, ans_count, _, _ = struct.unpack("!HHHHHH", res[:12])
        
        if (rxid != xid):
            continue
        else:
            check += 1
        
    
    print(res)
    print()
    print(rxid, hex(flags), qd_count, ans_count)
    
    # Skip Question
    
    i = 12
    i = skip_bytes(res, i)
    i += 4 # skip QTYPE (2 bytes) and QCLASS (2 bytes)
    i = skip_bytes(res, i)
    
    rtype, rclass, ttl, rdlength = struct.unpack("!HHIH", res[i:i+10])
    print(rtype, rclass, ttl, rdlength)
    
    ip_addr = res[i+10:i+10+rdlength]
    print('.'.join(str(p) for p in ip_addr))