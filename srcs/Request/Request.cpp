/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:23:15 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/10 12:55:46 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(Client *client): _client(client), _raw(""), _method(""), _uri(""), _version(""), _path(""), _query(""), _currentHeaderKey(""), _statusCode(-1), _state(0), _i(0){}

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
        this->_query = rhs._query;
        this->_currentHeaderKey = rhs._currentHeaderKey;
        // this->_body = rhs._body;
        this->_statusCode = rhs._statusCode;
        this->_headers = rhs._headers;   
    }
    return (*this);
}

Request::~Request(){}

const std::string &Request::getMethod() const
{
    return (_method);
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
        this->_raw += str;
        int boucleK = 0;
        int boucleV = 0;


        if (_state == START) 
            parseMethod(_state, _i, _raw);
        if (_state == URI) 
            parseUri(_state, _i,_raw);
        // if (_state == ) QUERY:
        //     parseQuery(_state, _raw);
        if (_state == VERSION) 
            parseVersion(_state, _i,_raw);
        // parse header qui boucle entre key et value jusqu'au body
        while (_state >= HEADER_KEY && _state <= HEADER_VALUE)
        {
            if (_state == HEADER_KEY)
            {
                parseHeaderKey(_state, _i,_raw);
                boucleK++;
                boucleV = 0;
            }
            if (_state == HEADER_VALUE)
            {
                parseHeaderValue(_state, _i,_raw);
                boucleV++;
                boucleK = 0;
            }
            if (boucleK > 3 || boucleV > 3)
                break;
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
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    while (idx < endLine && str[idx] != ' ')
    {
        _method += str[idx];
        idx++;
    }

    LogManager::log(LogManager::DEBUG, ("Http method: " + _method).c_str());

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
    LogManager::log(LogManager::DEBUG, "Parse uri");
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
    
    LogManager::log(LogManager::DEBUG, ("Http uri: " + _uri).c_str());

    state = VERSION;
}

void Request::parseVersion(int & state, size_t & idx, std::string const & str)
{
    LogManager::log(LogManager::DEBUG, "Parse version");
    size_t endLine = str.find("\r\n", idx);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    _version = str.substr(idx, endLine - idx);

    idx = endLine + 2; // Passer "\r\n"
    if (_version != "HTTP/1.1")
        throw std::runtime_error("ERROR: Bad Version");
    LogManager::log(LogManager::DEBUG, ("Http version: " + _version).c_str());
    state = HEADER_KEY;
}

//ANCIENNE VERSION

// void Request::parseHeaderKey(int & state, size_t & idx, std::string const & str)
// {
//     LogManager::log(LogManager::DEBUG, "parse header key");

//     std::cout << str[idx];
//     size_t endLine = str.find("\r\n", idx);
//     if (endLine == std::string::npos)
//         throw std::runtime_error("ERROR : invalid request format (missing CRLF)");

//     size_t colonPos = str.find(':', idx);
//     if (colonPos == std::string::npos || colonPos > endLine)
//         throw std::runtime_error("ERROR : invalid header format (missing ':')");

//     // Extraire la clé (header key)
//     std::string key = str.substr(idx, colonPos - idx);
//     idx = colonPos + 1; // Passer le ':'

//     // Ignorer les espaces après le ':'
//     while (idx < endLine && str[idx] == ' ')
//         idx++;

//     // Ajouter la clé à l'état interne si nécessaire
//     LogManager::log(LogManager::DEBUG, "Header_key: %s", key.c_str());
//     _currentHeaderKey = key; // Stocker temporairement la clé
    
//     state = HEADER_VALUE;
// }

// void Request::parseHeaderValue(int & state, size_t & idx, std::string const & str)
// {
//     LogManager::log(LogManager::DEBUG, "Parse header value");

//     size_t endLine = str.find("\r\n", idx);
//     if (endLine == std::string::npos)
//         throw std::runtime_error("ERROR : invalid request format (missing CRLF)");

//     // Extraire la valeur (header value)
//     std::string value = str.substr(idx, endLine - idx);
//     idx = endLine + 2; // Passer "\r\n"

//     // Ajouter la clé-valeur au map des headers
//     _headers[_currentHeaderKey] = value;

//     LogManager::log(LogManager::DEBUG, "Header_value: %s", value.c_str());

//     // Vérifier si on continue avec d'autres headers ou si on passe au corps
//     if (str[idx] == '\r' && str[idx + 1] == '\n') // Fin des headers
//     {
//         idx += 2; // Passer "\r\n"
//         state = END; // Passer à l'état BODY
//     }
//     else
//         state = HEADER_KEY; // Continuer avec un autre header
// }

// void    Request::displayValue()
// {
//     std::cout << _method << std::endl;    
//     std::cout << _uri << std::endl;    
//     std::cout << _version << std::endl;
//     _state = END;
// }

// NOUVELLE VERSION 
void Request::parseHeaderKey(int & state, size_t & idx, std::string const & str)
{
    LogManager::log(LogManager::DEBUG, "Parse HeaderKey");
    if (str.empty())
        throw std::runtime_error("ERROR : empty request");
    
    // size_t endLine = str.find("\r\n", idx);
    // if (endLine != std::string::npos)
    if (str[idx] == '\r' && str[idx + 1] == '\n')
    {
        std::cout << "END" << std::endl;
        state = END; // SUITE CHANGE EN BODY
        return ;
    }

        
    size_t colonPos = str.find(":", idx);
    if (colonPos == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    while (idx < colonPos)
    {
        _currentHeaderKey += str[idx];
        idx++;
    }
    idx += 2;
    LogManager::log(LogManager::DEBUG, ("HeaderKey " + _currentHeaderKey).c_str());

    state = HEADER_VALUE;
}

void Request::parseHeaderValue(int & state, size_t & idx, std::string const & str)
{
    LogManager::log(LogManager::DEBUG, "Parse HeaderValue");
    if (str.empty())
        throw std::runtime_error("ERROR : empty request");

    size_t endLine = str.find("\r\n", idx);
    if (endLine == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    std::string _currentHeaderValue;
    while (idx < endLine)
    {
        _currentHeaderValue += str[idx];
        idx++;
    }

    idx += 2;
    LogManager::log(LogManager::DEBUG, ("HeaderValue " + _currentHeaderValue).c_str());
    _headers[_currentHeaderKey] = _currentHeaderValue;
    _currentHeaderKey.clear();
    state = HEADER_KEY;
}


