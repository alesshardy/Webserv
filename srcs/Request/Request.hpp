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
    CGI,
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
        BlocServer                          *_matchingServer;// inint
        BlocLocation                        *_matchingLocation;
        std::string                         _raw;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _version;
        std::map<std::string, std::string>  _query;
        std::map<std::string, std::string>  _headers;
        std::string                         _currentHeaderKey;
        int                                 _statusCode;
        int                                 _state;
        int                                 _inHeader;
        size_t  _i;
        bool _isChunked;
        unsigned long long _maxBodySize;
        unsigned long long _contentLength;
        std::time_t _timeOut;
        bool _isCgi;
        

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
        void parseQuery();
        void parseBody();
        void clearProcessedData(size_t processedBytes);
        bool isTimeoutExceeded() const;
        void handleError(int code, int state, const std::string& errorMessage);
        void parseCgi();
        
        bool isCgi() {return _isCgi;}

        const std::string &getMethod() const {return _method;}
        const std::string &getUri() const {return _uri;}
        const std::string &getVersion() const {return _version;}
        const int &getStatusCode() const {return _statusCode;}
        const std::map<std::string, std::string> &getHeaders() const {return _headers;}
        const std::map<std::string, std::string> &getQuery() const {return _query;}
        const int &getState() const {return _state;}
        RequestBody *getBody() const {return _body;}
        std::string getUriExtension() const;
        BlocServer *getMatchingServer() const {return _matchingServer;}
        BlocLocation *getMatchingLocation() const {return _matchingLocation;}
        void setCode(int const &code) {_statusCode = code;}
        void setState(int const &state) {_state = state;}
};

#endif