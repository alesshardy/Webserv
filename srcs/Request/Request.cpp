/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpassin <tpassin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:23:15 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/04 13:17:24 by tpassin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(): _method(""), _path(""), _query(""), _uri(""), _protocol(""), /*_body("")*/ _statusCode(-1){}

Request::Request(Request const & copy)
{
    *this = copy;
}

Request & Request::operator=(Request const & rhs)
{
    if (this != &rhs)
    {
        this->_method = rhs._method;
        this->_uri = rhs._uri;
        this->_protocol = rhs._protocol;
        this->_path = rhs._path;
        this->_body = rhs._body;
        this->_statusCode = rhs._statusCode;
        this->_headers = rhs._headers;   
    }
    return (*this);
}

Request::~Request(){}

const std::string &Request::getMethod() const
{
    std::string tmp;
    
    tmp += getMethod() + getUri() + getProtocol();
    return (tmp);
}

const std::string &Request::getUri() const
{
    return (this->_uri);
}

const std::string &Request::getProtocol() const
{
    return (this->_protocol);
}

const std::string &Request::getPath() const
{
    return (this->_path);
}

const int &Request::getStatusCode() const
{
    return (this->_statusCode);
}

const std::map<std::string, std::string> &Request::getHeaders() const
{
    return (this->_headers);
}

const std::string &Request::getQuery() const
{
    return (this->_query);
}

void Request::setCode(int const code){this->_statusCode = code;}