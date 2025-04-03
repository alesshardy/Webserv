/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpassin <tpassin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:05:33 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/03 19:36:17 by tpassin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestBody.hpp"
#include <iostream>
#include <map>

class Request{
    private:
        std::string _method;
        std::string _uri;
        std::string _protocol;
        std::string _path;
        std::string _query;
        // std::string _body; // Class Request ou a gerer comme une string ?
        std::map<std::string, std::string> _headers;
        int _statusCode;
        RequestBody _body;
        
    public:
        Request();
        Request(Request const & copy);
        Request & operator=(Request const & rhs);
        ~Request();
        
        void parseRequest(void);
        void parseHeader(void);
        void parseProtocol(void);
        void parsePath(void);
        void parseQuery(void);
        void parseHeader(void);
        // void parseBody(void);
        
        
        const std::string                           &getMethod() const;
        const std::string                           &getUri() const;
        const std::string                           &getProtocol() const;
        const std::string                           &getPath() const;
        const std::string                           &getQuery() const;
        const int                                   &getStatusCode() const;
        const std::map<std::string, std::string>    &getHeaders() const;
        
        void setCode(int code);
};

#endif