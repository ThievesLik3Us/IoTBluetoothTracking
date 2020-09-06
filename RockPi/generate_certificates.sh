# Script to run in bash to generate ssl certificates for both server and client
#!/bin/bash
# Enter bluecate.com for the common name on the certificates

# Generate Client Certificates
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout server.key -out server.crt
# the file gets messed up so rename it
mv client.crt^M client.crt
# Generate Server Certificates
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout client.key -out client.crt
# the file gets messed up so rename it
mv server.crt^M server.crt
