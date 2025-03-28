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
    delete _client_socket;
    // delete _request;
    // delete _response;
}



