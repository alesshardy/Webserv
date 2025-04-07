/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpassin <tpassin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:05:33 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/07 19:30:13 by tpassin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestBody.hpp"
#include "Client.hpp"
#include <iostream>
#include <map>

class Client;

class Request{
    friend class Client;
    friend class Server;
    
    private:
        Client                              *_client;
        std::string                         _method;
        std::string                         _uri;
        std::string                         _protocol;
        std::string                         _path;
        std::string                         _query;
        std::map<std::string, std::string>  _headers;
        int                                 _statusCode;
        // Server                              *_server;
        // BlocLocation                        *_location;
        // RequestBody                         _body;

    public:                     
        Request(Client *client);                      
        Request(Request const & copy);
        Request & operator=(Request const & rhs);
        ~Request();
        
        void parseRequest(std::string request);
        // void parseHeader(void);
        // void parseProtocol(void);
        // void parsePath(void);
        // void parseQuery(void);
        // void parseBody(void);
        
        
        const std::string                           getMethod() const;
        const std::string                           &getUri() const;
        const std::string                           &getProtocol() const;
        const std::string                           &getPath() const;
        const std::string                           &getQuery() const;
        const int                                   &getStatusCode() const;
        const std::map<std::string, std::string>    &getHeaders() const;
        
        void setCode(int const code);
};

#endif