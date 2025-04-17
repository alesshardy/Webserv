#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestBody.hpp"
#include "Client.hpp"
#include <iostream>
#include <map>
#include <string>
#include <ctime>

#include "Server.hpp"
#include <sstream>
# define URI_MAX_SIZE 2048
# define REQUETE_LINE_MAX_SIZE 8192


enum parseState
{
    START,
    URI,
    VERSION,
    QUERY,
    HEADER_KEY,
    HEADER_VALUE,
    HEADER_CHECK,
    BODY,
    END,
    ERROR
};


class Client;
class Server;
class RequestBody;

class Request{
    friend class Client;
    friend class Server;
    
    private:
        Client                              *_client;
        Server                              *_server;
        RequestBody                         *_body;
        std::string                         _raw;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _version;
        std::string                         _path;
        std::map<std::string, std::string>  _query;
        std::map<std::string, std::string>  _headers;
        std::string                         _currentHeaderKey;
        int                                 _statusCode;
        int                                 _state;
        int                                 _error;
        // BlocLocation                        *_location;
        size_t  _i;
        bool _isChunked;
        unsigned long long _maxBodySize;
        unsigned long long _contentLength;
        std::time_t _timeOut;
        

    public:                     
        Request(Client *client, Server *server);                      
        Request(Request const & copy);
        Request & operator=(Request const & rhs);
        ~Request();
        
        void parseRequest(std::string request);
        void parseMethod();
        void parseUri(void);
        void parseHeaderKey(void);
        void parseHeaderValue(void);
        void parseVersion(void);
        void parseHeader(void);
        void checkHeader(void);
        void parseHeaderKeyValue(const std::string &headerKey, const std::string &headerValue);
        void getMaxBodySize();
        // void parseQuery();
        void parseBody();
        void clearProcessedData(size_t processedBytes);
        bool isTimeoutExceeded() const;


        
        // void parsePath(int & state, int & idx, std::string const & str);
        // void parseBody(int & state, int & idx, std::string const & str);
        
        const std::string                           &getMethod() const;
        const std::string                           &getUri() const;
        const std::string                           &getVersion() const;
        const std::string                           &getPath() const;
        const std::map<std::string, std::string>    &getQuery() const;
        const int                                   &getStatusCode() const;
        const std::map<std::string, std::string>    &getHeaders() const;
        const int                                   &getState() const;
        
        void setCode(int const & code);
        void setError(int const & error);
        void setState(int const & state);
};

#endif