from enum import Enum, auto
import socket
import json
import time
import sys
import io


class HttpState(Enum):
    START = auto()
    HEADERS = auto()
    BODY = auto()
    END = auto()


class HttpRequest(object):
    def __init__(self) -> None:
        self.headers = {}
        self.residual = b""
        self.state = HttpState.START
        self.body = b""

    def parse(self, data):
        bs = io.BytesIO(self.residual + data)

        if self.state is HttpState.START:
            request_line = bs.readline()
            if request_line[-1:] != b"\n":
                self.residual = request_line
                return
            self.method, self.uri, self.version = request_line.rstrip().split(b" ")
            self.state = HttpState.HEADERS

        if self.state is HttpState.HEADERS:
            while True:
                field_line = bs.readline()
                if not field_line:
                    break
                if field_line[-1:] != b"\n":
                    self.residual = field_line
                    return

                if field_line == b"\r\n" or field_line == b"\n":
                    if self.method == b"GET":
                        self.state = HttpState.END
                    else:
                        self.state = HttpState.BODY
                    break
                field_name, field_value = field_line.rstrip().split(b": ")
                self.headers[field_name.lower()] = field_value
        if self.state is HttpState.BODY:
            self.body += bs.read()


def should_keepalive(req):
    c = req.headers.get("connection")
    if req.version == b"HTTP/1.0":
        return c and c.lower() == b"keep-alive"
    elif req.version == b"HTTP/1.1":
        return not (c and c.lower() == b"close")


SERVER_ADDR = ("127.0.0.1", 7799)


def handle_client_connection(client_sock):
    while True:
        upstream_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        upstream_sock.connect(SERVER_ADDR)
        print(f"Connected to {SERVER_ADDR}")

        req = HttpRequest()
        close = False
        while req.state is not HttpState.END:
            data = client_sock.recv(4096)
            print(f"-> *       {len(data)}B")
            if not data:
                close = True
                break
            req.parse(data)
            upstream_sock.send(data)
            print(f"   * ->    {len(data)}B")

        if close:
            upstream_sock.close()
            break

        while True:
            res = upstream_sock.recv(4096)
            print(f"   * <-    {len(res)}B")
            if not res:
                break
            client_sock.send(res)
            print(f"<- *       {len(res)}B")

        upstream_sock.close()
        print(should_keepalive(req))
        if not should_keepalive(req):
            return


if __name__ == "__main__":
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("0.0.0.0", 7777))

    s.listen()
    print("Listening for connections...")

    while True:
        try:
            client_sock, addr = s.accept()

            print(f"New connection at {addr}")
            handle_client_connection(client_sock)
            client_sock.close()
        except KeyboardInterrupt:
            s.close()
            print("Exiting...")
            sys.exit()
        # except Exception as msg:
        #     client_sock.send(b"HTTP/1.1 500 Internal Server Error\r\n\r\n")
        #     print(msg)
        finally:
            client_sock.close()
