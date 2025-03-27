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

void Server::init(int port)
{

}