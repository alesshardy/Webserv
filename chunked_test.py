import socket

host = "localhost"
port = 4343

# Requête HTTP avec body chunked
request = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost:4343\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\n"
    "Wiki\r\n"
    "5\r\n"
    "pedia\r\n"
    "C\r\n"
    " in\r\nchunks.\r\n"
    "0\r\n"
    "\r\n"
)

# Envoyer la requête
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))
    s.sendall(request.encode())
    response = s.recv(4096)
    print(response.decode())

# import socket

# host = "localhost"
# port = 4343

# # Requête HTTP avec body en mode Content-Length
# request = (
#     "POST / HTTP/1.1\r\n"
#     "Host: localhost:4343\r\n"
#     "Content-Length: 20\r\n"
#     "\r\n"
#     "Wikipedia in\r\nchunks."
# )

# # Envoyer la requête
# with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#     s.connect((host, port))
#     s.sendall(request.encode())
#     response = s.recv(4096)
#     print(response.decode())