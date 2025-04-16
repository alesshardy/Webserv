#include "Client.hpp"


//Constructeur

Client::Client(int client_fd, Socket* client_socket, Server* server)
{
    _client_fd = client_fd;
    _client_socket = client_socket;
    _server = server;
    _request = new Request(this, _server);
    LogManager::log(LogManager::DEBUG, "Create request");
    _requestFinish = false;
    _response = new Response(this);
    _server = server;
}

Client::~Client()
{
    if (this->_request)
    {
        LogManager::log(LogManager::DEBUG, "Destroying request");
        delete this->_request;
    }
    if (this->_response)
    {
        LogManager::log(LogManager::DEBUG, "Destroying response");
        delete this->_response;
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
        if (_request->getState() == END)
        {
            _server->change_epoll_event(_client_fd, RESPONSE_EVENTS);
            LogManager::log(LogManager::INFO, "Request complete for client %d", _client_fd);
            // Passer à la gestion de la réponse
        }
        else
            return ;
            // LogManager::log(LogManager::DEBUG, "Request not complete for client %d", _client_fd);
    }
    catch (const std::exception &e)
    {
        LogManager::log(LogManager::ERROR, "Error handling request for client %d: \n%s", _client_fd, e.what());
        // Gérer l'erreur (fermer le client, envoyer une réponse d'erreur, etc.)
    }
}


void    Client::handleResponse(int epoll_fd)
{
    if (_response == NULL)
    {
        LogManager::log(LogManager::ERROR, "Response object is NULL for client %d", _client_fd);
        return;
    }

    (void)epoll_fd;
    LogManager::log(LogManager::DEBUG, "Handling response for client %d", _client_fd);
    LogManager::log(LogManager::DEBUG, "epoll_fd: %d", epoll_fd);

    // Send the response to the client
    if (_response->buildResponse(epoll_fd) == -1)
    {
        LogManager::log(LogManager::ERROR, "Failed to build response for client %d", _client_fd);
        return;
    }
    // Send the response to the client
    if (send(_client_fd, _response->getResponseHeader().c_str(), _response->getResponseHeader().size(), 0) == -1)
    {
        LogManager::log(LogManager::ERROR, "Failed to send response header to client %d", _client_fd);
        return;
    }
    if (send(_client_fd, _response->getResponseBody().c_str(), _response->getResponseBody().size(), 0) == -1)
    {
        LogManager::log(LogManager::ERROR, "Failed to send response body to client %d", _client_fd);
        return;
    }
    LogManager::log(LogManager::DEBUG, "Response sent to client %d", _client_fd);

    _server->change_epoll_event(_client_fd, REQUEST_EVENTS); // Revenir à l'état de lecture
    // Close the client socket after sending the response
    close(_client_fd);
    if (_request)
    {
        delete _request;
        _request = NULL;
    }
    if (_response)
    {
        delete _response;
        _response = NULL;
    }

    _request = new Request(this, _server);
    _response = new Response(this);
    
}
