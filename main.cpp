#include <iostream>
#include "LogManager.hpp"
#include "Socket.hpp"
#include <cstdlib> 
#include <csignal>
#include "./includes/Webserv.hpp"
#include "Config.hpp"
#include "Server.hpp"

bool running = true;

void signalHandler(int signum)
{
    (void)signum;
    
    LogManager::log(LogManager::INFO, "Interrupt signal received, shutting down...");
    running = false;
}

int main(int ac, char **av)
{
    // A enlever plus tard car config par default acceptable si pas de file
    if (ac != 2) 
    {
        std::cerr << "Usage:./webserv [configuration file] " << std::endl;
        return (EXIT_FAILURE);
    }
    
    // INIT LOGGER
    LogManager::setLogStatus(true);
    //LogManager::setLogFileStatus(true);
    LogManager::setLogConsoleStatus(true);

    //parsing fichier config (validation et recuperation des données)
    try {
        Config config;
        config.parseConfigFile(av[1], config);
        LogManager::log(LogManager::INFO, "Fichier config correct!");
        // print config ranger
        config.printConfig();
    }
    catch (const std::exception &e) {
        LogManager::log(LogManager::ERROR, e.what());
        return (EXIT_FAILURE);
    }

    // LogManager::log(LogManager::DEBUG, "Server is starting...");

    // // Capturer les signaux d'interruption pour arrêter proprement le serveur
    // signal(SIGINT, signalHandler);
    // signal(SIGTERM, signalHandler);

    // //test socket
    // try
    // {
    //     Socket socket(7070, "127.0.0.1");
    //     LogManager::log(LogManager::INFO, "Socket created successfully");

    //     // Vérifier le descripteur de fichier du socket
    //     if (socket.get_socket_fd() == -1)
    //     {
    //         LogManager::log(LogManager::ERROR, "Invalid socket file descriptor");
    //         return EXIT_FAILURE;
    //     }

    //     // Vérifier la liaison du socket
    //     struct sockaddr_in addr = socket.get_addr();
    //     if (addr.sin_port != htons(7070) || addr.sin_addr.s_addr != inet_addr("127.0.0.1"))
    //     {
    //         LogManager::log(LogManager::ERROR, "Socket binding failed");
    //         return EXIT_FAILURE;
    //     }

    //     LogManager::log(LogManager::INFO, "Socket is listening on port 7070");

    //     // Boucle principale pour maintenir le programme en cours d'exécution
    //     while (running)
    //     {
    //         // Attendre une connexion entrante pour tester l'acceptation
    //         // int client_fd = socket.accept_socket();
    //         // if (client_fd == -1)
    //         // {
    //         //     LogManager::log(LogManager::ERROR, "Failed to accept connection");
    //         //     return EXIT_FAILURE;
    //         // }

    //         // LogManager::log(LogManager::INFO, "Connection accepted successfully");
    //         // close(client_fd);

    //         // Attendre un court instant pour éviter une boucle trop rapide
    //         // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //         usleep(100000);
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     LogManager::log(LogManager::ERROR, e.what());
    //     return EXIT_FAILURE;
    // }

    // try
    // {
    //     // Créer une configuration valide
    //     Config config;

    //     // Ajouter un premier serveur
    //     BlocServer server1;
    //     server1.addListen(8080); // Ajouter un port
    //     server1.addListen(9090); // Ajouter un autre port
    //     server1.setRoot("/var/www/html");
    //     server1.addIndex("index.html");
    //     config.addServer(server1);

    //     // Ajouter un deuxième serveur
    //     BlocServer server2;
    //     server2.addListen(7070); // Ajouter un port
    //     server2.setRoot("/var/www/another");
    //     server2.addIndex("home.html");
    //     config.addServer(server2);

    //     // Initialiser le serveur
    //     Server server;
    //     server.set_config(config); // Assigner la configuration au serveur
    //     server.init();             // Initialiser le serveur

    //     LogManager::log(LogManager::INFO, "Server initialized successfully %d", server.get_socket());

    //     // Capturer les signaux pour arrêter proprement le serveur
    //     signal(SIGINT, signalHandler);
    //     signal(SIGTERM, signalHandler);

    //     // Lancer le serveur
    //     server.run();
    //     LogManager::log(LogManager::DEBUG, "fin de run");

    //     // Maintenir le programme en cours d'exécution pour tester les sockets
    //     // while (running)
    //     // {
    //     //     const std::map<int, Client*> &clients = server.get_clients_map();
    //     //     LogManager::log(LogManager::INFO, "Number of connected clients: %d", clients.size());

    //     //     for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    //     //     {
    //     //         LogManager::log(LogManager::INFO, "Client FD: %d", it->first);
    //     //     }
    //     //     // Simuler une boucle principale (vous pouvez ajouter des fonctionnalités ici plus tard)
    //     //     usleep(100000); // Pause de 100ms pour éviter une boucle trop rapide
    //     // }


    //     // Arrêter le serveur
    //     server.stop();
       

    //     // Vérifiez si le programme doit quitter
    //     if (!running)
    //     {
    //         LogManager::log(LogManager::INFO, "Exiting program...");
    //         return EXIT_SUCCESS;
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     LogManager::log(LogManager::ERROR, e.what());
    //     return EXIT_FAILURE;
    // }

    // //fermeture du serveur
    
    // LogManager::log(LogManager::INFO, "Server is shutting down...");
    
    return (EXIT_SUCCESS);
}

