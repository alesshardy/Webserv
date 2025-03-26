#include <iostream>
#include <cstdlib>
#include "srcs/LogManager/LogManager.hpp"
#include "srcs/Config/Config.hpp"

int main(int ac, char **av)
{
    // A enlever plus tard car config par default acceptable si pas de file
    if (ac != 2) 
    {
        std::cerr << "Usage:./webserv [configuration file] " << std::endl;
        return (EXIT_FAILURE);
    }
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
    
    LogManager::setLogStatus(true);
    LogManager::setLogFileStatus(true);
    LogManager::setLogConsoleStatus(true);

    LogManager::log(LogManager::DEBUG, "Server is starting...");
    
    //creation du serveur : socket et de epoll pour surveiler les sockets
        // gestion client (creer/ajouter nouveau socket a epoll)

    //boucle principale
        // requete
        // reponse

    //fermeture du serveur
    
    return (0);
}