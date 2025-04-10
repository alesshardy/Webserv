/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpassin <tpassin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:23:15 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/10 13:21:30 by tpassin          ###   ########.fr       */
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

        if (_state == START) 
            parseMethod();
        if (_state == URI) 
            parseUri();
        // if (_state == ) QUERY:
        //     parseQuery(_state, _raw);
        if (_state == VERSION) 
            parseVersion();
        // parse header qui boucle entre key et value jusqu'au body
        if (_state == HEADER_KEY || _state == HEADER_VALUE)
            parseHeader();
        // if (_state == BODY)
        //     parseBody();
    }
    catch (const std::runtime_error &e)
    {
        LogManager::log(LogManager::ERROR, e.what());
        throw; // Rethrow the exception if needed
    }
}

void Request::parseMethod()
{
    LogManager::log(LogManager::DEBUG, "Parse method");
    if (_raw.empty())
        throw std::runtime_error("ERROR : empty request");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    while (_i < endLine && _raw[_i] != ' ')
    {
        _method += _raw[_i];
        _i++;
    }

    LogManager::log(LogManager::DEBUG, ("Http method: " + _method).c_str());

    if (_method != "GET" && _method != "POST" && _method != "DELETE" && _method != "PUT")
        throw std::runtime_error("ERROR : unsupported HTTP method");

    while (_i < endLine && _raw[_i] == ' ')
        _i++;

    if (_i >= endLine)
        throw std::runtime_error("ERROR: Missing URI after method");

    _state = URI;
}

void Request::parseUri()
{
    LogManager::log(LogManager::DEBUG, "Parse uri");
    while (_i < _raw.size() && _raw[_i] == ' ')
        _i++;

    if (_i >= _raw.size() || _raw[_i] != '/')
        throw std::runtime_error("ERROR: Uri unknown format");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    while (_i < endLine && _raw[_i] != ' ')
    {
        _uri += _raw[_i];
        _i++;
    }

    while (_i < endLine && _raw[_i] == ' ')
        _i++;

    if (_i >= endLine)
        throw std::runtime_error("ERROR: Missing Version after uri");
    
    LogManager::log(LogManager::DEBUG, ("Http uri: " + _uri).c_str());

    _state = VERSION;
}

void Request::parseVersion()
{
    LogManager::log(LogManager::DEBUG, "Parse version");
    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    _version = _raw.substr(_i, endLine - _i);

    _i = endLine + 2; // Passer "\r\n"
    if (_version != "HTTP/1.1")
        throw std::runtime_error("ERROR: Bad Version");
    LogManager::log(LogManager::DEBUG, ("Http version: " + _version).c_str());
    _state = HEADER_KEY;
}

//ANCIENNE VERSION

// void Request::parseHeaderKey()
// {
//     LogManager::log(LogManager::DEBUG, "parse header key");

//     std::cout << _raw[_i];
//     size_t endLine = _raw.find("\r\n", _i);
//     if (endLine == std::string::npos)
//         throw std::runtime_error("ERROR : invalid request format (missing CRLF)");

//     size_t colonPos = _raw.find(':', _i);
//     if (colonPos == std::string::npos || colonPos > endLine)
//         throw std::runtime_error("ERROR : invalid header format (missing ':')");

//     // Extraire la clé (header key)
//     std::string key = _raw.substr(_i, colonPos - _i);
//     _i = colonPos + 1; // Passer le ':'

//     // Ignorer les espaces après le ':'
//     while (_i < endLine && _raw[_i] == ' ')
//         _i++;

//     // Ajouter la clé à l'état interne si nécessaire
//     LogManager::log(LogManager::DEBUG, "Header_key: %s", key.c_str());
//     _currentHeaderKey = key; // Stocker temporairement la clé
    
//     _state = HEADER_VALUE;
// }

// void Request::parseHeaderValue()
// {
//     LogManager::log(LogManager::DEBUG, "Parse header value");

//     size_t endLine = _raw.find("\r\n", _i);
//     if (endLine == std::string::npos)
//         throw std::runtime_error("ERROR : invalid request format (missing CRLF)");

//     // Extraire la valeur (header value)
//     std::string value = _raw.substr(_i, endLine - _i);
//     _i = endLine + 2; // Passer "\r\n"

//     // Ajouter la clé-valeur au map des headers
//     _headers[_currentHeaderKey] = value;

//     LogManager::log(LogManager::DEBUG, "Header_value: %s", value.c_str());

//     // Vérifier si on continue avec d'autres headers ou si on passe au corps
//     if (_raw[_i] == '\r' && _raw[_i + 1] == '\n') // Fin des headers
//     {
//         _i += 2; // Passer "\r\n"
//         _state = END; // Passer à l'état BODY
//     }
//     else
//         _state = HEADER_KEY; // Continuer avec un autre header
// }

// void    Request::displayValue()
// {
//     std::cout << _method << std::endl;    
//     std::cout << _uri << std::endl;    
//     std::cout << _version << std::endl;
//     _state = END;
// }

// NOUVELLE VERSION 
void Request::parseHeaderKey()
{
    LogManager::log(LogManager::DEBUG, "Parse HeaderKey");
    if (_raw.empty())
        throw std::runtime_error("ERROR : empty request");
    
    // size_t endLine = _raw.find("\r\n", _i);
    // if (endLine != std::string::npos)
    if (_raw[_i] == '\r' && _raw[_i + 1] == '\n')
    {
        if (this->_method == "GET")
            _state = END; // SUITE CHANGE EN BODY
        else if (this->_method == "PUT" || this->_method == "POST")
            _state = BODY;
        return ;
    }

    size_t colonPos = _raw.find(":", _i);
    if (colonPos == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    while (_i < colonPos)
    {
        _currentHeaderKey += _raw[_i];
        _i++;
    }
    _i += 2;
    LogManager::log(LogManager::DEBUG, ("HeaderKey " + _currentHeaderKey).c_str());

    _state = HEADER_VALUE;
}

void Request::parseHeaderValue()
{
    LogManager::log(LogManager::DEBUG, "Parse HeaderValue");
    if (_raw.empty())
        throw std::runtime_error("ERROR : empty request");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    std::string _currentHeaderValue;
    while (_i < endLine)
    {
        _currentHeaderValue += _raw[_i];
        _i++;
    }

    _i += 2;
    LogManager::log(LogManager::DEBUG, ("HeaderValue " + _currentHeaderValue).c_str());
    _headers[_currentHeaderKey] = _currentHeaderValue;
    _currentHeaderKey.clear();
    _state = HEADER_KEY;
}

void   Request::parseHeader()
{
    int boucleK = 0;
    int boucleV = 0;

    while (_state >= HEADER_KEY && _state <= HEADER_VALUE)
    {
        if (_state == HEADER_KEY)
        {
            parseHeaderKey();
            boucleK++;
            boucleV = 0;
        }
        if (_state == HEADER_VALUE)
        {
            parseHeaderValue();
            boucleV++;
            boucleK = 0;
        }
        if (boucleK > 3 || boucleV > 3)
            break;
    }
}


