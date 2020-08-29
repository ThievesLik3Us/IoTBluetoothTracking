# RockPi Files
______________________________________
rockpi_client.py - creates a SSL encrypted socket connection with the server and sends a JSON object as raw bytes
rockpi_server.py - creates a SSL encrypted socket connection and waits for a client to connect and send a JSON object
generate_certs.sh - linux script that generates the certs needed for the above scripts to establish an SSL connection
