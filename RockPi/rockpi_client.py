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


def initialize_socket():
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

    return client_connection

def send_json_object(client_connection, json_object):
    print("Sending: JSON Test object")
    message = json.dumps(json_object)
    # Send the JSON object in byte format
    client_connection.sendall(message.encode())

def close_connection():
    print("Closing connection")
    client_connection.close()

# Create Test JSON object and convert it to raw bytes
json_test_object =  { "name":"John", "age":30, "city":"New York" }
client_connection = initialize_socket()
send_json_object(client_connection, json_test_object)
close_connection()
