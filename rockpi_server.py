import socket
import ssl
# Setup Server address and port
HOST_ADDR = '127.0.0.1'
HOST_PORT = 8081

# Setup Client address and port
LISTEN_ADDR = '127.0.0.1'
LISTEN_PORT = 8081

# Setup Client Certificates and key files
SERVER_SSL_CERT = "server_cert.pem"
SERVER_SSL_KEY = "server_key.key"
SERVER_SNI_HOSTNAME = "example.com"

# Setup Client Certificates and key files
CLIENT_SSL_CERT = "client_cert.pem"
CLIENT_SSL_KEY = "client_key.key"


# Client side socket listener with SSL certificates
def Client_SSL_Handler()
    # Establish Client certificates and Server authentication certificates
    client_context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=SERVER_SSL_CERT)
    client_context.load_cert_chain(certfile=CLIENT_SSL_CERT, keyfile=CLIENT_SSL_KEY)

    # Create Client socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Wrap the socket in SSL and establish it as a client
    client_connection = context.wrap_socket(client_socket, server_side=False, server_hostname=SERVER_SNI_HOSTNAME)
    # Establish connection to the host address and port
    client_connection.connect((HOST_ADDR, HOST_PORT))

    print("SSL established. Peer: {}".format(client_connection.getpeercert()))

    print("Sending: 'Hello, world!")
    client_connection.send(b"Hello, world!")

    print("Closing connection")
    client_connection.close()

# Server side socket listener with SSL certificates
def Server_SSL_Handler()
    # Wrap the socket in SSL certs
    # Purpose.CLIENT_AUTH loads CA certificates for client certificate verification on the server side.
    server_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
    # Require an SSL cert in the connection
    server_context.verify_mode = ssl.CERT_REQUIRED
    # Load the Server side certificates
    server_context.load_cert_chain(certfile=SERVER_SSL_CERT, keyfile=SERVER_SSL_KEY)
    # Validate Client certficate
    server_context.load_verify_locations(cafile=CLIENT_SSL_CERT)

    # Create a socket on the host to a specific port
    server_bindsocket = socket.socket()
    server_bindsocket.bind((HOST_ADDR, HOST_PORT))
    server_bindsocket.listen(5)

    while True:
        print("Waiting for client")
        newsocket, fromaddr = server_bindsocket.accept()
        print("Client connected: {}:{}".format(fromaddr[0], fromaddr[1]))
        server_connection = context.wrap_socket(newsocket, server_side=True)
        print("SSL established. Peer: {}".format(server_connection.getpeercert()))
        client_data_buffer = b''  # Buffer to hold received client data
        try:
            while True:
                received_data = server_connection.recv(4096)
                if received_data:
                    # Client sent us data. Append to buffer
                    client_data_buffer += received_data
                else:
                    # No more data from client. Show buffer and close connection.
                    print("Received:", client_data_buffer)
                    break
        finally:
            print("Closing connection")
            server_connection.shutdown(socket.SHUT_RDWR)
            server_connection.close()
