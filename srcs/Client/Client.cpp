#include "Client.hpp"


//Constructeur

Client::Client(int client_fd, Socket* client_socket)
{
    _client_fd = client_fd;
    _client_socket = client_socket;
    _request = new Request(this);
    LogManager::log(LogManager::DEBUG, "Create request");
       _requestFinish = false;
    // _response = new Response();
}



Client::~Client()
{
    if (this->_request)
    {
        LogManager::log(LogManager::DEBUG, "Destroying request");
        delete this->_request;
    }
    LogManager::log(LogManager::DEBUG, "Destroying client %d", _client_fd);
    // Ne pas supprimer _client_socket ici, il est géré par Server
    // delete _request;
    // delete _response;
}


void Client::handleRequest(std::string const & str)
{
    try
    {
        // LogManager::log(LogManager::DEBUG, "Handling request for client %d", _client_fd);

        // Parsing Requete
        _request->parseRequest(str);

        // Vérifier si la requête est complète
        // if (_request->getState() == END)
        // {
        //     LogManager::log(LogManager::INFO, "Request complete for client %d", _client_fd);
        //     // Passer à la gestion de la réponse
        // }
        // else
        //     LogManager::log(LogManager::DEBUG, "Request not complete for client %d", _client_fd);
    }
    catch (const std::exception &e)
    {
        LogManager::log(LogManager::ERROR, "Error handling request for client %d: %s", _client_fd, e.what());
        // Gérer l'erreur (fermer le client, envoyer une réponse d'erreur, etc.)
    }
}


void    Client::handleResponse(int epoll_fd)
{
    (void)epoll_fd;
    LogManager::log(LogManager::DEBUG, "Handling response for client %d", _client_fd);
    LogManager::log(LogManager::DEBUG, "epoll_fd: %d", epoll_fd);
    // _response->sendResponse();
}
