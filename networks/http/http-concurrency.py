import select
import socket
from parser import HttpRequest, HttpState


def should_keepalive(req):
    c = req.headers.get("connection")
    if req.version == b"HTTP/1.0":
        return c and c.lower() == b"keep-alive"
    elif req.version == b"HTTP/1.1":
        return not (c and c.lower() == b"close")


OWN_ADDR = ("0.0.0.0", 7777)
SERVER_ADDR = ("127.0.0.1", 7799)

if __name__ == "__main__":
    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.setblocking(False)
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listener.bind(OWN_ADDR)

    listener.listen(10)
    print("Listening for connections...")

    inputs = [listener]
    outputs = []
    upstream_for_client = {}
    req_for_client = {}
    to_send_client = {}

    while True:
        # try:
        readable, writable, exceptional = select.select(inputs, outputs, inputs)

        for s in readable:
            if s is listener:
                client_sock, client_addr = s.accept()
                print(f"Client connected at addr {client_addr}")
                client_sock.setblocking(False)
                inputs.append(client_sock)
            else:
                try:
                    upstream_sock = upstream_for_client[s]
                except:
                    upstream_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    upstream_sock.connect(SERVER_ADDR)
                    print(f"Connected to {SERVER_ADDR}")
                    upstream_for_client[s] = upstream_sock

                try:
                    req = req_for_client[s]
                except:
                    req = HttpRequest()
                    req_for_client[s] = req

                data = s.recv(4096)
                print(f"-> *       {len(data)}B")
                if not data:
                    s.close()
                    inputs.remove(s)
                    del upstream_for_client[s]
                    del req_for_client[s]
                else:
                    req.parse(data)
                    upstream_sock.send(data)
                    print(f"   * ->    {len(data)}B")

                if req.state is HttpState.END:
                    while True:
                        res = upstream_sock.recv(4096)
                        print(f"   * <-    {len(res)}B")
                        if not res:
                            break
                        s.send(res)
                        print(f"<- *       {len(res)}B")

                    upstream_sock.close()
                    del upstream_for_client[s]
                    del req_for_client[s]

                    if not should_keepalive(req):
                        s.close()
                        inputs.remove(s)

        # except KeyboardInterrupt:
        #     listener.close()
        #     print("Exiting...")
        #     sys.exit()
        # finally:
        #     client_sock.close()
