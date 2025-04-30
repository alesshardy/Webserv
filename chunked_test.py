import socket

host = "localhost"
port = 4343

# Requête HTTP avec body chunked
request = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost:4343\r\n"
    "Content-Length: 20\r\n"
    "\r\n"
    "Wikipedia in chunks."
)

# Envoyer la requête
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))
    s.sendall(request.encode())
    response = s.recv(4096)
    print(response.decode())