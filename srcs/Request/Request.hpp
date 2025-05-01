#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestBody.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "CgiRequest.hpp"
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <sstream>
#include <filesystem>

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
class CgiRequest;

class Request{
    friend class Client;
    friend class Server;
    friend class CgiRequest;
    friend class Response;

    
    private:
        Client                              *_client;
        Server                              *_server;
        RequestBody                         *_body;
        BlocServer                          *_matchingServer;// siuuu inint
        BlocLocation                        *_matchingLocation;// siuuu inint
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
        size_t                              _i;
        bool                                _isChunked;
        unsigned long long                  _maxBodySize;
        unsigned long long                  _contentLength;
        std::time_t                         _timeOut;
        bool                                _isCgi;
        std::string                         _queryString;
        int                                 _port;
        CgiRequest                          *_cgi;
        
        // Method
        void parseRequest(std::string request);
        void parseMethod();
        void parseUri(void);
        void parseHeaderKey(void);
        void parseHeaderValue(void);
        void parseVersion(void);
        void parseHeader(void);
        void checkHeader(void);
        void parseHeaderKeyValue(const std::string &headerKey, const std::string &headerValue);
        void parseQuery();
        void parseBody();
        void clearProcessedData(size_t processedBytes);
        void handleError(int code, int state, const std::string& errorMessage);
        void parseCgi();
        void getMaxBodySize();
        
        // Check avant body
        void checkHostHeader();
        void findMatchingServerAndLocation();
        void checkCgi();
        void validateContentLengthAndEncoding();
        void skipHeaderEndSequence();
        void checkAllowedMethods();
        
        void handleFileTransfer();
        
        // utils
        bool isTimeoutExceeded() const;
        bool isCgi() {return _isCgi;}
        std::string getUriExtension() const;

    public:                     
        Request(Client *client, Server *server);                      
        Request(Request const & copy);
        Request & operator=(Request const & rhs);
        ~Request();
        
        // Getters
        const std::string &getMethod() const {return _method;}
        const std::string &getUri() const {return _uri;}
        const std::string &getVersion() const {return _version;}
        const int &getStatusCode() const {return _statusCode;}
        const std::map<std::string, std::string> &getHeaders() const {return _headers;}
        const std::pair<std::string, std::string> getHeader(const std::string &key) const
        {
            std::map<std::string, std::string>::const_iterator it = _headers.find(key);
            if (it != _headers.end())
                return *it;
            return std::make_pair("", "");
        }
        const std::map<std::string, std::string> &getQuery() const {return _query;}
        const int &getState() const {return _state;}
        RequestBody *getBody() const {return _body;}
        BlocServer *getMatchingServer() const {return _matchingServer;}
        BlocLocation *getMatchingLocation() const {return _matchingLocation;}
        const int &getPort() const {return _port;}
        const std::string &getQueryString() const {return _queryString;}
        CgiRequest *getCgi() const {return _cgi;}


    
        // Setters
        void setCode(int const &code) {_statusCode = code;}
        void setState(int const &state) {_state = state;}
        void setPort(int const &port) {_port = port;}

    };

#endif