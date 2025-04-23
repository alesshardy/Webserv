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
#include "Client.hpp"
#include "ErrorPage.hpp"

enum    r_state
{
    R_INIT,
    R_PROCESSING,
    R_CHUNK,
    R_END
};

class   Client;
class   Request;
class   Server;

class   Response
{
    friend class Client;
    friend class Request;
    friend class Server;


    private:
        int                  _client_fd;
        Socket*              _client_socket;
        Request*             _request;
        Server*              _server;
        Client*              _client;
        std::string          _response;
        std::string          _response_body;
        std::string          _response_header;
        std::string          _response_status;
        std::string          _response_code;
        int                 _r_state;
        std::time_t         _timeOut;


        //methodes
        void                _handleGet();
        void                _sendFullResponse(const std::string& filePath, const std::string& contentType);
        void                _sendChunkedResponse(const std::string& filePath, const std::string& contentType);
        void                _handlePost();
        void                _handleDelete();
        void                _handlePut();
        void                _handleHead();

    public:
        Response(Client* client);
        ~Response();


        int                 buildResponse(int epoll_fd);
        
        void                setResponseHeader(const std::string& header);
        void                setResponseBody(const std::string& body);
        void                setResponseStatus(const std::string& status);
        void                setResponseCode(const std::string& code);
        void                setRequest(Request* request);
        void                setServer(Server* server);
        void                setClientFd(int client_fd);
        void                setClientSocket(Socket* client_socket);
        void                setTimeStartResponse(){_timeOut = std::time(NULL);};

        std::string         resolveFilePath();

        // Getters
        int                 getClientFd() const { return _client_fd; }
        Socket*             getClientSocket() const { return _client_socket; }
        Request*            getRequest() const { return _request; }
        Server*             getServer() const { return _server; }
        std::string         getResponse() const { return _response; }
        std::string         getResponseBody() const { return _response_body; }
        std::string         getResponseHeader() const { return _response_header;}
        const int           &getResponseState() const {return (this->_r_state);}

        void                setRState(int state) { _r_state = state;}
        

        bool                isTimeoutExceeded() const;

        // void                handleError(int error_code, bool errorPage = true);
};

#endif