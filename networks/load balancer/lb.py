import socket


class LoadBalancer:
    def __init__(self):
        self.servers = []  # List of server IPs or hostnames
        self.current_index = 0  # Index of the last server used

    def add_server(self, server):
        self.servers.append(server)

    def next_server(self):
        if len(self.servers) == 0:
            raise Exception("No servers available.")

        server = self.servers[self.current_index]
        self.current_index = (self.current_index + 1) % len(self.servers)
        return server

    def handle_client(self, client_socket):
        server = self.next_server()

        # Connect to the selected server
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.connect((server, 8000))  # Assuming port 8000 for the server

        # Forward client data to the server
        while True:
            data = client_socket.recv(4096)
            if not data:
                break
            server_socket.sendall(data)

        # Forward server response back to the client
        while True:
            data = server_socket.recv(4096)
            if not data:
                break
            client_socket.sendall(data)

        # Close the connections
        client_socket.close()
        server_socket.close()

    def start(self):
        # Create a listening socket for clients
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind(
            ("localhost", 9000)
        )  # Assuming port 9000 for the load balancer
        server_socket.listen(10)

        print("Load balancer started. Listening on port 9000...")

        while True:
            # Accept client connections
            client_socket, address = server_socket.accept()
            print(f"Client connected from {address[0]}:{address[1]}")

            # Handle client connection in a separate thread or process
            self.handle_client(client_socket)


# Example usage
lb = LoadBalancer()
lb.add_server("192.168.0.1")
lb.add_server("192.168.0.2")
lb.add_server("192.168.0.3")
lb.start()
