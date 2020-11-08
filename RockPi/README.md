# RockPi Files
rockpi_client.py - creates a SSL encrypted socket connection with the server and sends a JSON object as raw bytes

rockpi_server.py - creates a SSL encrypted socket connection and waits for a client to connect and send a JSON object

generate_certs.sh - linux script that generates the certs needed for the above scripts to establish an SSL connection

encrypt_objects.py - takes in a JSON object and produces an AES256 encrpyted cypher text using Tink

decrypt_objects.py - takes in an AES256 encrypted cypher text and decrypts it to produce a JSON object using tink (assuming the same key is used)
