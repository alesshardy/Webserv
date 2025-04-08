#include "Client.hpp"


//Constructeur

Client::Client(int client_fd, Socket* client_socket)
{
    _client_fd = client_fd;
    _client_socket = client_socket;
    // _request = new Request();
    // _response = new Response();
}

Client::~Client()
{
    LogManager::log(LogManager::DEBUG, "Destroying client %d", _client_fd);
    // Ne pas supprimer _client_socket ici, il est géré par Server
    // delete _request;
    // delete _response;
}


void    Client::handleRequest()
{
    LogManager::log(LogManager::DEBUG, "Handling request for client %d", _client_fd);
}



void    Client::handleResponse(int epoll_fd)
{
    LogManager::log(LogManager::DEBUG, "Handling response for client %d", _client_fd);
    // _response->sendResponse();
}
