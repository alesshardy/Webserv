/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:05:33 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/10 19:00:13 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestBody.hpp"
#include "Client.hpp"
#include <iostream>
#include <map>
#include <string>

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

class Request{
    friend class Client;
    friend class Server;
    
    private:
        Client                              *_client;
        Server                              *_server;
        std::string                         _raw;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _version;
        std::string                         _path;
        std::string                         _query;
        std::map<std::string, std::string>  _headers;
        std::string                         _currentHeaderKey;
        int                                 _statusCode;
        int                                 _state;
        // BlocLocation                        *_location;
        // RequestBody                         _body;
        size_t  _i;
        bool _isChunked;
        unsigned long long _maxBodySize;
        unsigned long long _contentLength;
        

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
        void parseBody();
        
        // void parsePath(int & state, int & idx, std::string const & str);
        // void parseQuery(int & state, int & idx, std::string const & str);
        // void parseBody(int & state, int & idx, std::string const & str);
        void displayValue(void);
        
        
        const std::string                           &getMethod() const;
        const std::string                           &getUri() const;
        const std::string                           &getVersion() const;
        const std::string                           &getPath() const;
        const std::string                           &getQuery() const;
        const int                                   &getStatusCode() const;
        const std::map<std::string, std::string>    &getHeaders() const;
        const int                                   &getState() const;
        
        void setCode(int const code);
};

#endif