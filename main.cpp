#include <iostream>
#include "LogManager.hpp"
#include "Socket.hpp"
#include <cstdlib> 
#include <csignal>
#include <Config.hpp>

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
    LogManager::setLogFileStatus(true);
    LogManager::setLogConsoleStatus(true);

    //parsing fichier config (validation et recuperation des données)
    try {
        Config config;
        config.parseConfigFile(av[1], config);
        LogManager::log(LogManager::INFO, "Fichier config correct!");
    }
    catch (const std::exception &e) {
         LogManager::log(LogManager::ERROR, e.what());
        return (EXIT_FAILURE);
    }

    LogManager::log(LogManager::DEBUG, "Server is starting...");

    // Capturer les signaux d'interruption pour arrêter proprement le serveur
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    //test socket
    try
    {
        Socket socket(7070, "127.0.0.1");
        LogManager::log(LogManager::INFO, "Socket created successfully");

        // Vérifier le descripteur de fichier du socket
        if (socket.get_socket_fd() == -1)
        {
            LogManager::log(LogManager::ERROR, "Invalid socket file descriptor");
            return EXIT_FAILURE;
        }

        // Vérifier la liaison du socket
        struct sockaddr_in addr = socket.get_addr();
        if (addr.sin_port != htons(7070) || addr.sin_addr.s_addr != inet_addr("127.0.0.1"))
        {
            LogManager::log(LogManager::ERROR, "Socket binding failed");
            return EXIT_FAILURE;
        }

        LogManager::log(LogManager::INFO, "Socket is listening on port 7070");

        // Boucle principale pour maintenir le programme en cours d'exécution
        while (running)
        {
            // Attendre une connexion entrante pour tester l'acceptation
            // int client_fd = socket.accept_socket();
            // if (client_fd == -1)
            // {
            //     LogManager::log(LogManager::ERROR, "Failed to accept connection");
            //     return EXIT_FAILURE;
            // }

            // LogManager::log(LogManager::INFO, "Connection accepted successfully");
            // close(client_fd);

            // Attendre un court instant pour éviter une boucle trop rapide
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            usleep(100000);
        }
    }
    catch (const std::exception &e)
    {
        LogManager::log(LogManager::ERROR, e.what());
        return EXIT_FAILURE;
    }

    //fermeture du serveur
    
    LogManager::log(LogManager::INFO, "Server is shutting down...");
    
    return (0);
}