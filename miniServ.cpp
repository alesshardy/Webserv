#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define PORT 8080

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 1️⃣ Créer le socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    // 2️⃣ Attacher le socket à une adresse IP et un port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accepter toutes les connexions
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Erreur bind");
        exit(EXIT_FAILURE);
    }

    // 3️⃣ Écouter les connexions
    if (listen(server_fd, 3) < 0) {
        perror("Erreur listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Serveur en attente de connexion sur le port " << PORT << std::endl;

    // 4️⃣ Accepter une connexion entrante
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Erreur accept");
        exit(EXIT_FAILURE);
    }

    // 5️⃣ Lire les données du client
    read(new_socket, buffer, 1024);
    std::cout << "Message reçu : " << buffer << std::endl;

    // 6️⃣ Répondre au client
    const char* response = "HTTP/1.1 200 OK\nContent-Length: 13\n\nHello, World!";
    send(new_socket, response, strlen(response), 0);

    // 7️⃣ Fermer la connexion
    close(new_socket);
    close(server_fd);

    return 0;
}
