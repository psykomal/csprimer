import socket
import json
import time
import sys

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind(("0.0.0.0", 7777))

s.listen()
print("Listening for connections...")


while True:
    try:
        conn, addr = s.accept()

        print(f"New connection at {addr}")

        req = conn.recv(4096)
        print(f"-> *       {len(req)}B")
        headers, body = req.split(b"\r\n\r\n")

        new_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        new_socket.connect(("127.0.0.1", 7799))
        # forward_headers = b'GET / HTTP/1.1\r\nHost: 127.0.0.1:7799\r\nUser-Agent: proxx\r\nAccept: */*'
        # new_req = forward_headers + b'\r\n\r\n' + body

        new_socket.send(req)
        print(f"   * ->    {len(req)}B")
        # new_socket.sendto(new_req, ("127.0.0.1", 7799))

        while True:
            res = new_socket.recv(4096)
            print(f"   * <-    {len(res)}B")
            if not res:
                break
            conn.send(res)

            print(f"<- *       {len(res)}B")

        new_socket.close()
        conn.close()
    except KeyboardInterrupt:
        s.close()
        print("Exiting...")
        sys.exit()
    except OSError as msg:
        print(msg)
    finally:
        new_socket.close()
        conn.close()
