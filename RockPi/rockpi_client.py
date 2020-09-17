#!/usr/bin/python3
import socket
import ssl
import json
from rockpi_retrieve_json_object import Retrieve_From_DB
from pymongo import MongoClient

# Setup Client address and port
LISTEN_ADDR = '10.0.0.7'
LISTEN_PORT = 8082

# Setup Client Certificates and key files
CLIENT_SSL_CERT = '../certs/client.crt'
CLIENT_SSL_KEY = '../certs/client.key'

CA_SSL_CERT = './certs/ca.crt'
CA_SSL_KEY = './certs/ca.key'

# Setup Client Certificates and key files
SERVER_SSL_CERT = '../certs/server.crt'
SERVER_SSL_KEY = '../certs/server.key'
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

    print("Super Secret SSL Handshake completed. Peer: {}".format(client_connection.getpeercert()))

    return client_connection

def send_json_object(client_connection, json_object):
    print("Sending: JSON Test object. Hope it works")
    del json_object['_id']
    print("\nJSON After Delete")
    # Create a string from the json_object
    message = json.dumps(json_object)
    # Send the JSON object in byte format
    client_connection.sendall(message.encode())

def close_connection():
    print("connection is closed ... for now")
    client_connection.close()

# Create Test JSON object and convert it to raw bytes
# json_test_object =  { "username":"myuser", "email":"test@aol.com", "password":"T3st123!" }
# Connect to Local MongoDB
mongodb_client = MongoClient(port=27017)
android_db = mongodb_client.AndroidDB

# Grab JSON object matching the criteria
json_test_object = android_db.AndroidCollection.find_one({'username': "myuser"})
print("\nJSON Object: ", json_test_object)

#json_test_object = Retrieve_From_DB('myuser')
client_connection = initialize_socket()
send_json_object(client_connection, json_test_object)
close_connection()
