// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstring>
// #include <cstdlib>
// #include <cstdio>

// #define PORT 8080

// int main()
// {
//     int server_fd, new_socket;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
//     char buffer[1024] = {0};

//     // 1️⃣ Créer le socket
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1) {
//         perror("Erreur socket");
//         exit(EXIT_FAILURE);
//     }

//     // 2️⃣ Attacher le socket à une adresse IP et un port
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;  // Accepter toutes les connexions
//     address.sin_port = htons(PORT);

//     if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
//         perror("Erreur bind");
//         exit(EXIT_FAILURE);
//     }

//     // 3️⃣ Écouter les connexions
//     if (listen(server_fd, 3) < 0) {
//         perror("Erreur listen");
//         exit(EXIT_FAILURE);
//     }

//     std::cout << "Serveur en attente de connexion sur le port " << PORT << std::endl;

//     // 4️⃣ Accepter une connexion entrante
//     new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
//     if (new_socket < 0) {
//         perror("Erreur accept");
//         exit(EXIT_FAILURE);
//     }

//     // 5️⃣ Lire les données du client
//     read(new_socket, buffer, 1024);
//     std::cout << "Message reçu : " << buffer << std::endl;

//     // 6️⃣ Répondre au client
//     const char* response = "HTTP/1.1 200 OK\nContent-Length: 13\n\nHello, World!";
//     send(new_socket, response, strlen(response), 0);

//     // 7️⃣ Fermer la connexion
//     close(new_socket);
//     close(server_fd);

//     return 0;
// }

[2025-04-14 15:18:52] [DEBUG] HeaderKey Priority
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] HeaderValue  u=6
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Request not complete for client 5
[2025-04-14 15:18:52] [DEBUG] Checking headers ...
[2025-04-14 15:18:52] [DEBUG] Extracted Host:  localhost, Port: 1234
[2025-04-14 15:18:52] [DEBUG] Max body size set to: 1048576
[2025-04-14 15:18:52] [DEBUG] Checking headers DONE
[2025-04-14 15:18:52] [INFO] Request complete for client 5
[2025-04-14 15:18:52] [INFO] Client 5 disconnected
[2025-04-14 15:18:52] [DEBUG] Closing client ACTUEL 5
[2025-04-14 15:18:52] [INFO] Closing client 5
[2025-04-14 15:18:52] [DEBUG] Destroying request
[2025-04-14 15:18:52] [DEBUG] Destroying response
[2025-04-14 15:18:52] [DEBUG] Destroying client 5
[2025-04-14 15:18:52] [DEBUG] EPOLLOUT event on socket 5
==573318== Invalid read of size 8
==573318==    at 0x41F26C: Client::getRequest() const (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==    by 0x41E428: Server::handleEpollEvents() (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==    by 0x41DE7D: Server::run() (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==    by 0x40516D: main (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==  Address 0x10 is not stack'd, malloc'd or (recently) free'd
==573318== 
==573318== 
==573318== Process terminating with default action of signal 11 (SIGSEGV)
==573318==  Access not within mapped region at address 0x10
==573318==    at 0x41F26C: Client::getRequest() const (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==    by 0x41E428: Server::handleEpollEvents() (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==    by 0x41DE7D: Server::run() (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==    by 0x40516D: main (in /home/tpassin/42/cercle_4/Webserv/WebServ)
==573318==  If you believe this happened as a result of a stack
==573318==  overflow in your program's main thread (unlikely but
==573318==  possible), you can try to increase the size of the
==573318==  main thread stack using the --main-stacksize= flag.
==573318==  The main thread stack size used in this run was 8388608.
==573318== 
==573318== HEAP SUMMARY:
==573318==     in use at exit: 583,912 bytes in 192 blocks
==573318==   total heap usage: 4,710 allocs, 4,518 frees, 3,006,994 bytes allocated
==573318== 
==573318== LEAK SUMMARY:
==573318==    definitely lost: 192 bytes in 4 blocks
==573318==    indirectly lost: 140,713 bytes in 84 blocks
==573318==      possibly lost: 0 bytes in 0 blocks
==573318==    still reachable: 443,007 bytes in 104 blocks
==573318==         suppressed: 0 bytes in 0 blocks
==573318== Rerun with --leak-check=full to see details of leaked memory
==573318== 
==573318== For lists of detected and suppressed errors, rerun with: -s
==573318== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
[1]    573318 segmentation fault (core dumped)  valgrind ./WebServ ConfFiles/good/default.conf --debug
