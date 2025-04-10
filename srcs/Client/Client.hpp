#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "LogManager.hpp"
#include "BlocServer.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"


class Request;
class Server;

class Client
{
    friend class Request;
    friend class Server;

    private:
        int                 _client_fd;
        Socket*              _client_socket;
        Request*             _request;
        Response*            _response;
        Server*     _server;
        bool   _requestFinish;

        



    public:
        Client(int client_fd, Socket* client_socket, Server *server);
        ~Client();

        // methodes 
        void                handleRequest(std::string const & str);
        void                handleResponse(int epoll_fd);     

        

        // Getters
        int                 getClientFd() const { return _client_fd; }
        Socket*             getClientSocket() const { return _client_socket; }
        Request*            getRequest() const { return _request; }
        Response*           getResponse() const { return _response; }


    
};

#endif