#!/usr/bin/python3
import socket
import struct
import sys
import time
import ssl
import json
import rockpi_client
from pymongo import MongoClient

# Setup Server address and port
HOST_ADDR = ""
HOST_PORT = 8082
print("IP/Port: " + HOST_ADDR + "/" + str(HOST_PORT))

def Multicast():
    message = b'very important data'
    multicast_group = ('224.3.29.71', 4446)

    # Create the datagram socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    # Set a timeout so the socket does not block indefinitely when trying
    # to receive data.
    #sock.settimeout(10)
    # Set the time-to-live for messages to 1 so they do not go past the
    # local network segment.
    ttl = struct.pack('b', 32)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

    try:
        # Send data to the multicast group
        print(sys.stderr, 'sending "%s"' % message)
        sent = sock.sendto(message, multicast_group)

        # Look for responses from all recipients
        while True:
            sent = sock.sendto(message, multicast_group)
            print(sys.stderr, 'waiting to receive')
            try:
                data, server = sock.recvfrom(16)
            except socket.timeout:
                print(sys.stderr, 'timed out, no more responses')
                break
            else:
                print(sys.stderr, 'received "%s" from %s' % (data, server))

    finally:
        print(sys.stderr, 'closing socket')
        sock.close()

def Process_Request(json_object):
    # Grab the command from the json object
    command = json_object['command']
    # remove the command from the json object
    del json_object['command']

    # Connect to Local MongoDB
    mongodb_client = MongoClient(port=27017)
    android_db = mongodb_client.AndroidDB

    if (command == 'Store'):
        # Store the JSON object into the AndroidDB database inside the AndroidCollection
        result = android_db.AndroidCollection.insert_one(client_json_object)
        print("Stored JSON object with result: ", result)
    elif (command == 'Retrieve'):
        # Grab JSON object matching the criteria
        retrieved_json_object = android_db.AndroidCollection.find_one(json_object)
        print("\nJSON Object: ", retrieved_json_object)
        del json_object['_id']
        print("\nJSON After Delete")
        Send_Message(retrieved_json_object)

#Multicast()
#UDP_Broadcast()

# Setup Client Certificates and key files
CLIENT_SSL_CERT = '../certs/client.crt'
CLIENT_SSL_KEY = '../certs/client.key'

CA_SSL_CERT = './certs/ca.crt'
CA_SSL_KEY = './certs/ca.key'

# Setup Client Certificates and key files
SERVER_SSL_CERT = '../certs/server.crt'
SERVER_SSL_KEY = '../certs/server.key'
SERVER_SNI_HOSTNAME = 'bluecate.com'

# Wrap the socket in SSL certs
# Purpose.CLIENT_AUTH loads CA certificates for client certificate verification on the server side.
server_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
# Require an SSL cert in the connection
server_context.verify_mode = ssl.CERT_REQUIRED
# Load the Server side certificates
server_context.load_cert_chain(certfile=SERVER_SSL_CERT, keyfile=SERVER_SSL_KEY)

# Validate Client certficate
#server_context.load_verify_locations(cafile=CA_SSL_CERT)
server_context.load_verify_locations(cafile=CLIENT_SSL_CERT)

# Create a socket on the host to a specific port
server_bindsocket = socket.socket()
server_bindsocket.bind((HOST_ADDR, HOST_PORT))
server_bindsocket.listen(5)
print("IP Addr: " + server_bindsocket.getsockname()[0])

while True:
    print("Waiting for the lazy ass client")
    newsocket, fromaddr = server_bindsocket.accept()
    print("Client finally showed up: {}:{}".format(fromaddr[0], fromaddr[1]))
    server_connection = server_context.wrap_socket(newsocket, server_side=True)
    print("Super Secret SSL Handshake completed. Peer: {}".format(server_connection.getpeercert()))
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

                # Process the Command sent
                Process_Request(client_json_object)
                break
    except KeyboardInterrupt:
        print("Keyboard input to shutdown program")

    finally:
        print("connection is closed ... for now")
        server_connection.shutdown(socket.SHUT_RDWR)
        server_connection.close()
