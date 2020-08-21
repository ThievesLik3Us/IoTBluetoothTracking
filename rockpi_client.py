#!/usr/bin/python3
import socket
import ssl
import json

# Setup Client address and port
LISTEN_ADDR = '127.0.0.1'
LISTEN_PORT = 8082

# Setup Client Certificates and key files
CLIENT_SSL_CERT = 'client.crt'
CLIENT_SSL_KEY = 'client.key'

# Setup Client Certificates and key files
SERVER_SSL_CERT = 'server.crt'
SERVER_SSL_KEY = 'server.key'
SERVER_SNI_HOSTNAME = 'bluecate.com'

# Establish Client certificates and Server authentication certificates
client_context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=SERVER_SSL_CERT)
client_context.load_cert_chain(certfile=CLIENT_SSL_CERT, keyfile=CLIENT_SSL_KEY)

# Create Client socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Wrap the socket in SSL and establish it as a client
client_connection = client_context.wrap_socket(client_socket, server_side=False, server_hostname=SERVER_SNI_HOSTNAME)
# Establish connection to the host address and port
client_connection.connect((LISTEN_ADDR, LISTEN_PORT))

print("SSL established. Peer: {}".format(client_connection.getpeercert()))

print("Sending: JSON Test object")
# create temp JSON object to test sending JSON
json_test_object =  { "name":"John", "age":30, "city":"New York" }
message = json.dumps(json_test_object)

# Send the JSON object in byte format
client_connection.sendall(message.encode())

print("Closing connection")
client_connection.close()
