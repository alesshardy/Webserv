/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:05:33 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/10 12:42:20 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestBody.hpp"
#include "Client.hpp"
#include <iostream>
#include <map>
#include <string>


enum parseState
{
    START,
    URI,
    VERSION,
    QUERY,
    HEADER_KEY,
    HEADER_VALUE,
    BODY,
    END,
    ERROR
};


class Client;

class Request{
    friend class Client;
    friend class Server;
    
    private:
        Client                              *_client;
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
        // Server                              *_server;
        // BlocLocation                        *_location;
        // RequestBody                         _body;
        size_t  _i;

    public:                     
        Request(Client *client);                      
        Request(Request const & copy);
        Request & operator=(Request const & rhs);
        ~Request();
        
        void parseRequest(std::string request);
        void parseMethod(int & state, size_t & idx, std::string const & str);
        void parseUri(int & state, size_t & idx, std::string const & str);
        void parseHeaderKey(int & state, size_t & idx, std::string const & str);
        void parseHeaderValue(int & state, size_t & idx, std::string const & str);
        void parseVersion(int & state, size_t & idx, std::string const & str);
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