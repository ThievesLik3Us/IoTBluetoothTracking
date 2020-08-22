#!/usr/bin/python3
import socket
import ssl
import json
import tink
from tink import hybrid


# Setup Server address and port
HOST_ADDR = '127.0.0.1'
HOST_PORT = 8082

# Setup Client Certificates and key files
CLIENT_SSL_CERT = 'client.crt'
CLIENT_SSL_KEY = 'client.key'

# Setup Client Certificates and key files
SERVER_SSL_CERT = 'server.crt'
SERVER_SSL_KEY = 'server.key'
SERVER_SNI_HOSTNAME = 'bluecate.com'

CLIENT_PUBLIC_KEY = ''

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

context = b'context'
# Register all Hybrid primitives
hybrid.register()

# 1. Generate the private key material.
private_keyset_handle = tink.new_keyset_handle(hybrid.hybrid_key_templates.ECIES_P256_HKDF_HMAC_SHA256_AES128_GCM) #??? Does this key get leaked if its created every time a message gets sent

# Obtain the public key material.
public_keyset_handle = private_keyset_handle.public_keyset_handle()
# Encryption

# 2. Get the primitive.
hybrid_encrypt = public_keyset_handle.primitive(hybrid.HybridEncrypt)

server_connection.sendall(hybrid_encrypt)

while True:
    print("Waiting for client")
    newsocket, fromaddr = server_bindsocket.accept()
    print("Client connected: {}:{}".format(fromaddr[0], fromaddr[1]))
    server_connection = server_context.wrap_socket(newsocket, server_side=True)
    print("SSL established. Peer: {}".format(server_connection.getpeercert()))
    client_data_buffer = b''  # Buffer to hold received client data
    received_public_key_flag = 0;
    try:
        while true:
            received_data = server_connection.recv(4096)
            if received_data:
                # Client sent us data. Append to buffer
                client_data_buffer += received_data
            elif received_public_key_flag == 0:
                CLIENT_PUBLIC_KEY = client_data_buffer
                received_public_key_flag = 1
            else:
                # No more data from client. Show buffer and close connection.
                # print("Received:", client_data_buffer.decode())
                # Create JSON object from received data

                # Decryption

                # 2. Get the primitive.
                hybrid_decrypt = private_keyset_handle.primitive(hybrid.HybridDecrypt)

                # 3. Use the primitive.
                unencrypted_json_object = hybrid_decrypt.decrypt(client_data_buffer, context)
                client_json_object = json.loads(unencrypted_json_object.decode())
                print("JSON Object:", client_json_object)
                break
    finally:
        print("Closing connection")
        server_connection.shutdown(socket.SHUT_RDWR)
        server_connection.close()
