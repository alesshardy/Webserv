#ifndef RESPONSE_H
#define RESPONSE_H


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>

#include "Request.hpp"
#include "LogManager.hpp"
#include "BlocServer.hpp"




class   Response
{
    private:
        int                 _client_fd;
        Socket*              _client_socket;
        Request*             _request;
        BlocServer*          _server;
        std::string          _response_body;
        std::string          _response_header;
        std::string          _response_status;
        std::string          _response_code;

        //methodes
        void                _handleGet();
        void                _handlePost();
        void                _handleDelete();
        void                _handlePut();
        void                _handleHead();

    public:
        Response(int client_fd, Socket* client_socket, Request* request, BlocServer* server);
        ~Response();


        int                 buildResponse(int epoll_fd);
        
        void                setResponseHeader(const std::string& header);
        void                setResponseBody(const std::string& body);
        void                setResponseStatus(const std::string& status);
        void                setResponseCode(const std::string& code);
        void                setRequest(Request* request);
        void                setServer(BlocServer* server);
        void                setClientFd(int client_fd);
        void                setClientSocket(Socket* client_socket);


};

#endif