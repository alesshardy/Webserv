# import socket

# host = "localhost"
# port = 4343

# # Requête HTTP avec body chunked
# request = (
#     "POST / HTTP/1.1\r\n"
#     "Host: localhost:4343\r\n"
#     "Content-Length: 20\r\n"
#     "\r\n"
#     "5\r\n"
#     "Wiki\r\n"
#     "5\r\n"
#     "pedia\r\n"
#     "C\r\n"
#     " in\r\nchunks.\r\n"
#     "0\r\n"
#     "\r\n"
# )

# # Envoyer la requête
# with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#     s.connect((host, port))
#     s.sendall(request.encode())
#     response = s.recv(4096)
#     print(response.decode())


import socket

host = "localhost"
port = 4343

# Requête HTTP avec body en mode Content-Length
request = (
    "POOST / HTTP/1.1\r\n"
    "Host: localhost:4343\r\n"
    "Content-Length: 21\r\n"
    "\r\n"
    "Wikipedia in\r\nchunks."
)

# Envoyer la requête
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))
    s.sendall(request.encode())
    response = s.recv(4096)
    print(response.decode())


# import socket

# host = "localhost"
# port = 3434

# # Contenu du body à envoyer
# body_content = "This is the body of the POST request."

# # Construire la requête HTTP
# request = (
#     f"POST /upload HTTP/1.1\r\n"
#     f"Host: {host}:{port}\r\n"
#     f"ContentLength: {len(body_content)}\r\n"
#     f"Content-Type: text/plain\r\n"
#     f"\r\n"
#     f"{body_content}"
# )

# # Envoyer la requête
# with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#     s.connect((host, port))
#     s.sendall(request.encode())
#     response = s.recv(4096)
#     print(response.decode())

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