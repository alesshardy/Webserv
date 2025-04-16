/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:21:35 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/16 18:07:45 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBody.hpp"

RequestBody::RequestBody(size_t maxBodySize, bool isChunked)
{   
    _bodyStr = "";
    _currentSize = 0;
    _maxBodySize = maxBodySize;
    _isChunked = isChunked;
    _isComplete = false;
}

RequestBody::~RequestBody()
{
}

void RequestBody::parseContentLength(const std::string &rawData, size_t  &index, size_t contentLength)
{
    if (_currentSize >= contentLength)
    {
        _isComplete = true;
        return ;
    }

    size_t bytesToRead = rawData.size() - index;

    if (bytesToRead > 0)
    {
        _bodyStr.append(rawData.substr(index, bytesToRead));
        _currentSize += bytesToRead;
        index += bytesToRead;
    }

    if (_currentSize == contentLength)
        _isComplete = true;
    else if (_currentSize > contentLength)
        throw std::runtime_error("ERROR: Body size exceeds Content-Length");
    
}

void RequestBody::parseChunked(const std::string &rawData, size_t &index)
{
    while (index < rawData.size())
    {
        // Étape 1 : Lire la taille du chunk
        size_t endSize = rawData.find("\r\n", index);
        if (endSize == std::string::npos)
            return; // Pas assez de données pour lire la taille du chunk

        std::string chunkSizeHex = rawData.substr(index, endSize - index);
        size_t chunkSize = 0;

        // Convertir la taille du chunk de l'hexadécimal en décimal
        std::istringstream iss(chunkSizeHex);
        iss >> std::hex >> chunkSize;
        
        // Avancer après la taille du chunk
        index = endSize + 2; // Sauter "\r\n"

        // Étape 2 : Vérifier la fin du body (chunk de taille 0)
        if (chunkSize == 0)
        {
            // Vérifier qu'on a le CRLF final
            if (index + 2 <= rawData.size() && rawData.substr(index, 2) == "\r\n")
            {
                index += 2; // Sauter le CRLF final
                _isComplete = true;
                return;
            }
            return; // Pas assez de données pour le CRLF final
        }

        // Étape 3 : Vérifier qu'on a le chunk entier
        if (index + chunkSize + 2 > rawData.size())
            return; // Pas assez de données pour le chunk complet

        // Vérifier qu'on ne dépasse pas la taille max
        if (_currentSize + chunkSize > _maxBodySize)
            throw std::runtime_error("ERROR: Chunked body exceeds maximum allowed size");
        
        // Ajouter le chunk au body
        _bodyStr.append(rawData.substr(index, chunkSize));
        _currentSize += chunkSize;

        // Vérifier que les deux caractères suivant le chunk sont "\r\n"
        if (rawData.substr(index + chunkSize, 2) != "\r\n")
            throw std::runtime_error("ERROR: Chunked size announce don't match reality");

        // Avancer au chunk suivant
        index += chunkSize + 2; // Sauter le chunk et "\r\n"]
    }
}

