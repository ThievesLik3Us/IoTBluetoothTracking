#!/usr/bin/python3
import socket
import ssl
import time

# Setup Client address and port
# LISTEN_ADDR = '10.0.0.167'
LISTEN_ADDR = '10.0.0.161'

# LISTEN_PORT = 8082
LISTEN_PORT = 8082


# Setup Client Certificates and key files
CLIENT_SSL_CERT = 'certs/client_android.crt'
CLIENT_SSL_KEY = 'certs/client_android.key'

# Setup Client Certificates and key files
SERVER_SSL_CERT = 'certs/server.crt'
SERVER_SSL_KEY = 'certs/server.key'

CA_SSL_CERT = 'certs/ca.crt'
SERVER_SNI_HOSTNAME = 'www.bluecate.com'

# Establish Client certificates and Server authentication certificates
client_context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=CA_SSL_CERT)
client_context.load_cert_chain(certfile=CLIENT_SSL_CERT, keyfile=CLIENT_SSL_KEY)

# Create Client socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Wrap the socket in SSL and establish it as a client
client_connection = client_context.wrap_socket(client_socket, server_side=False, server_hostname=SERVER_SNI_HOSTNAME)
# Establish connection to the host address and port
client_connection.connect((LISTEN_ADDR, LISTEN_PORT))

print("SSL established. Peer: {}".format(client_connection.getpeercert()))

message = "Hello, world! From Client";
print("Sending: ", message)
client_connection.sendall(message.encode())
print("Client Message Sent")

data = client_connection.recv(1024)
print("Received", str(data.decode()))


print("Closing connection")
client_connection.close()
