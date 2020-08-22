#!/usr/bin/python3
import socket
import ssl
import json
import tink
from tink import hybrid

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


# Create Test JSON object and convert it to raw bytes
json_test_object =  { "name":"John", "age":30, "city":"New York" }
message = json.dumps(json_test_object)
context = b'context'

server_public_key = client_connection.recv(4096)

print("Encrypting JSON Object")
# Register all Hybrid primitives
hybrid.register()

# 1. Generate the private key material.
private_keyset_handle = tink.new_keyset_handle(hybrid.hybrid_key_templates.ECIES_P256_HKDF_HMAC_SHA256_AES128_GCM)

# Obtain the public key material.
public_keyset_handle = private_keyset_handle.public_keyset_handle()

# Encryption

# 2. Get the primitive.
hybrid_encrypt = public_keyset_handle.primitive(hybrid.HybridEncrypt)

# Send the public key to the server to use for encrypting the message
client_connection.sendall(hybrid_encrypt)

# Receive the public key from the server to encrypt messages sent to the server


# 3. Use the primitive.
ciphertext = hybrid_encrypt.encrypt(message.encode(), context)


print("Sending: JSON Test object")

# Send the JSON object in byte format
client_connection.sendall(ciphertext)

print("Closing connection")
client_connection.close()
