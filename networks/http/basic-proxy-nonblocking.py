import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 65432  # Port to listen on

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    s.setblocking(False)

    while True:
        try:
            conn, addr = s.accept()
        except BlockingIOError:
            pass  # No connections ready yet, so do other things
        else:
            print("Connected by", addr)

            while True:
                try:
                    data = conn.recv(1024)
                except BlockingIOError:
                    pass  # No data available yet
                else:
                    if not data:
                        break  # Client disconnected
                    print(data)

            print("Closing connection")
            conn.close()
