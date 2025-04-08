/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpassin <tpassin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:23:15 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/08 17:00:23 by tpassin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(Client *client): _client(client), _method(""), _uri(""), _version(""), _path(""), _query(""), _statusCode(-1){}

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
        this->_version = rhs._version;
        this->_path = rhs._path;
        // this->_body = rhs._body;
        this->_statusCode = rhs._statusCode;
        this->_headers = rhs._headers;   
    }
    return (*this);
}

Request::~Request(){}

const std::string Request::getMethod() const
{
    std::string tmp;
    
    tmp += this->_method + getUri() + getVersion();
    return (tmp);
}

const std::string &Request::getUri() const
{
    return (this->_uri);
}

const std::string &Request::getVersion() const
{
    return (this->_version);
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

const int &Request::getState() const
{
    return (_state);
}

void Request::parseRequest(std::string str)
{
    try
    {
        _state = START;
        size_t i = 0;

        while (str[i] && _state != END && _state != ERROR)
        {
            switch (_state)
            {
                case START:
                    parseMethod(_state, i, str);
                    break;
                case URI:
                    parseUri(_state, i, str);
                    break;
                // case QUERY:
                //     parseQuery(_state, i, str);
                    break;
                case VERSION:
                    parseVersion(_state, i, str);
                    break;
                // case HEADER_KEY:
                //     parseHeader(_state, i, str);
                //     break;
                // case HEADER_VALUE:
                //     parseHeader(_state, i, str);
                //     break;
                // case BODY:
                //     parseBody(_state, i, str);
                //     break;
                case END:
                    displayValue();
                    break;
                default:
                    throw std::runtime_error("Invalid state");
            }
        }
    }
    catch (const std::runtime_error &e)
    {
        LogManager::log(LogManager::ERROR, e.what());
        throw; // Rethrow the exception if needed
    }
}

void Request::parseMethod(int & state, size_t & idx, std::string const & str)
{
    LogManager::log(LogManager::DEBUG, "Parse method");
    if (str.empty())
        throw std::runtime_error("ERROR : empty request");

    size_t endLine = str.find("\r\n", idx);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    while (idx < endLine && str[idx] != ' ')
    {
        _method += str[idx];
        idx++;
    }

    LogManager::log(LogManager::DEBUG, ("HTTP Method: " + _method).c_str());

    if (_method != "GET" && _method != "POST" && _method != "DELETE" && _method != "PUT")
        throw std::runtime_error("ERROR : unsupported HTTP method");

    while (idx < endLine && str[idx] == ' ')
        idx++;

    if (idx >= endLine)
        throw std::runtime_error("ERROR: Missing URI after method");

    state = URI;
}

void Request::parseUri(int & state, size_t & idx, std::string const & str)
{
    while (idx < str.size() && str[idx] == ' ')
        idx++;

    if (idx >= str.size() || str[idx] != '/')
        throw std::runtime_error("ERROR: Uri unknown format");

    size_t endLine = str.find("\r\n", idx);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    while (idx < endLine && str[idx] != ' ')
    {
        _uri += str[idx];
        idx++;
    }

    while (idx < endLine && str[idx] == ' ')
        idx++;

    if (idx >= endLine)
        throw std::runtime_error("ERROR: Missing Version after uri");

    state = VERSION;
}

void Request::parseVersion(int & state, size_t & idx, std::string const & str)
{
    size_t endLine = str.find("\r\n", idx);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    _version = str.substr(idx, endLine - idx);

    idx = endLine + 2; // Passer "\r\n"
    state = END;
}

void    Request::displayValue()
{
    std::cout << _method << std::endl;    
    std::cout << _uri << std::endl;    
    std::cout << _version << std::endl;
}

