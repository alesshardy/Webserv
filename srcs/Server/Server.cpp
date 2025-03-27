#include "Server.hpp"


//Constructeur
Server::Server()
{
    _state = STOPPED;
    _socket = -1;
    _epoll_fd = -1;
}
//Destructeur
Server::~Server()
{
    close_all();
}


//Initialisation du serveur

void Server::init()
{

    //creation de l'instance epoll
    _epoll_fd = epoll_create1(O_CLOEXEC);
    if (_epoll_fd == -1)
    {
        LogManager::log(LogManager::ERROR, "Error creating epoll instance");
        return;
    }

    //Recuperation des confugurations des serveurs
    std::vector<BlocServer> servers = _config.getServers();

    for (size_t i = 0; i < servers.size(); i++)
    {
        //creation de la socket
        Socket socket()
    }

}