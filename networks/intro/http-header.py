import socket
import json

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind(('0.0.0.0', 7777))

s.listen()
print("Listening for connections...")

while True:
    
    conn, addr = s.accept()
    
    print(f"New connection at {addr}")
    
    req = conn.recv(4096)
    
    headers, body = req.split(b'\r\n\r\n')
    
    data = {}
    for header in headers.split(b'\r\n')[1:]:
        k, v = header.split(b': ')
        data[k.decode('utf-8')] = v.decode('utf-8')
    
    conn.send(b'HTTP/1.1 200 OK\r\n\r\n')
    conn.send(json.dumps(data, indent=4).encode('utf-8'))
    
    conn.close()
    