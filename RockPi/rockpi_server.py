#!/usr/bin/python3
import socket
import ssl
import json

# Setup Server address and port
hostname = socket.gethostname()
ip_address = socket.gethostbyname(hostname)

HOST_ADDR = '10.0.0.167'
HOST_PORT = 8082
print("IP/Port" + HOST_ADDR + "/" + str(HOST_PORT))

# Setup Client Certificates and key files
CLIENT_SSL_CERT = 'client.crt'
CLIENT_SSL_KEY = 'client.key'

CA_SSL_CERT = 'ca.crt'
CA_SSL_KEY = 'ca.key'

# Setup Client Certificates and key files
SERVER_SSL_CERT = 'server.crt'
SERVER_SSL_KEY = 'server.key'
SERVER_SNI_HOSTNAME = 'www.bluecate.com'

# Wrap the socket in SSL certs
# Purpose.CLIENT_AUTH loads CA certificates for client certificate verification on the server side.
server_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
# Require an SSL cert in the connection
server_context.verify_mode = ssl.CERT_REQUIRED
# Load the Server side certificates
server_context.load_cert_chain(certfile=SERVER_SSL_CERT, keyfile=SERVER_SSL_KEY)

# Validate Client certficate
server_context.load_verify_locations(cafile=CA_SSL_CERT)
# server_context.load_verify_locations(cafile=CLIENT_SSL_CERT)

# Create a socket on the host to a specific port
server_bindsocket = socket.socket()
server_bindsocket.bind((HOST_ADDR, HOST_PORT))
server_bindsocket.listen(5)

while True:
    print("Waiting for client")
    newsocket, fromaddr = server_bindsocket.accept()
    print("Client connected: {}:{}".format(fromaddr[0], fromaddr[1]))
    try:
        server_connection = server_context.wrap_socket(newsocket, server_side=True)
        print("SSL established. Peer: {}".format(server_connection.getpeercert()))
    except ssl.SSLError as e:
        print(e)

    client_data_buffer = b''  # Buffer to hold received client data
    try:
        server_connection.send("Hello From Server".encode())
        while True:
            received_data = server_connection.recv(4096)
            if received_data:
                # Client sent us data. Append to buffer
                client_data_buffer += received_data
                print(client_data_buffer)

            else:
                # No more data from client. Show buffer and close connection.
                print("Received:", client_data_buffer.decode())
                # Create JSON object from received data
                client_json_object = json.loads(client_data_buffer.decode())
                print("JSON Object:", client_json_object)
                break
    except KeyboardInterrupt:
        print("Keyboard input to shutdown program")

    finally:
        print("Closing connection")
        server_connection.shutdown(socket.SHUT_RDWR)
        server_connection.close()
