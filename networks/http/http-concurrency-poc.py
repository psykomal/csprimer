import socket
import select

BIND_ADDR = ("0.0.0.0", 7777)


if __name__ == "__main__":
    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.setblocking(False)
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listener.bind(BIND_ADDR)

    listener.listen(10)

    inputs = [listener]
    outputs = []
    to_send = set()

    while True:
        readable, writable, exceptional = select.select(inputs, outputs, inputs)

        for s in readable:
            if s is listener:
                client_sock, client_addr = s.accept()
                client_sock.setblocking(False)
                inputs.append(client_sock)
            else:
                data = s.recv(4096)
                if data:
                    print(data)
                    outputs.append(s)
                    to_send.add(s)
                else:
                    s.close()
                inputs.remove(s)

        for w in writable:
            if w in to_send:
                w.send(b"HTTP/1.0 200 OK\r\n\r\nHi!")
                to_send.remove(w)
                outputs.remove(w)
                w.close()
