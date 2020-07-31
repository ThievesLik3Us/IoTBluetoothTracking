import socket, ssl

context = ssl.SSLContext(ssl.PROTOCOL_TLSv1)
context.load_cert_chain(certfile="cert.pem", keyfile="cert.pem")

bindsocket = socket.socket()
bindsocket.bind(('', 2099))
bindsocket.listen(5)

while True:
     newsocket, fromaddr = bindsocket.accept()
     connstream = ssl.wrap_socket(newsocket,
                             server_side=True,
                             certfile="cert.pem",
                             keyfile="cert.pem",
                             ssl_version=ssl.PROTOCOL_TLSv1) 
     request = sslsoc.read()
     print(request)