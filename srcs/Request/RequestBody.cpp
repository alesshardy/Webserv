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

void RequestBody::parseChunked(const std::string &rawData, size_t  &index)
{
    while (index < rawData.size())
    {
        size_t endSize = rawData.find("\r\n", index);
        if (endSize == std::string::npos)
            return ; // pas assez de data, continue a lire

        
    }
}

void RequestBody::parseContentLength(const std::string &rawData, size_t  &index, size_t contentLength)
{
    if (_currentSize >= contentLength)
    {
        _isComplete = true;
        return ;
    }

    size_t bytesToRead = rawData.size() - index;
    // size_t remainBytes = contentLength - _currentSize;

    if (bytesToRead > 0)
    {
        _bodyStr.append(rawData.substr(index, bytesToRead));
        _currentSize += bytesToRead;
        index += bytesToRead;
    }

    if (_currentSize >= contentLength)
        _isComplete = true;
    
}

// void RequestBody::parseChunked(const std::string &rawData, size_t &index)
// {
//     // size_t start_index = index;
    
//     while (index < rawData.size())
//     {
//         // Lire la taille du chunk
//         size_t endSize = rawData.find("\r\n", index);
//         if (endSize == std::string::npos)
//             return; // Pas assez de données, continuer à lire
        
//         std::string chunkSizeHex = rawData.substr(index, endSize - index);
//         size_t chunkSize = 0;
        
//         // Convertir de l'hexadécimal en décimal
//         std::istringstream iss(chunkSizeHex);
//         iss >> std::hex >> chunkSize;
        
//         // Avancer après la taille du chunk
//         index = endSize + 2; // Sauter "\r\n"
        
//         // Fin du body si chunk size = 0
//         if (chunkSize == 0)
//         {
//             // Vérifier qu'on a le CRLF final
//             if (index + 2 <= rawData.size() && rawData.substr(index, 2) == "\r\n")
//             {
//                 index += 2; // Sauter le CRLF final
//                 _isComplete = true;
//                 return;
//             }
//             return; // Pas assez de données, attendre le CRLF final
//         }
        
//         // Vérifier qu'on a le chunk entier
//         if (index + chunkSize + 2 > rawData.size())
//             return; // Pas assez de données
        
//         // Vérifier qu'on ne dépasse pas la taille max
//         if (_currentSize + chunkSize > _maxBodySize)
//             throw std::runtime_error("ERROR: Chunked body exceeds maximum allowed size");
        
//         // Ajouter le chunk au body
//         _bodyStr.append(rawData.substr(index, chunkSize));
//         _currentSize += chunkSize;
        
//         // Avancer au chunk suivant
//         index += chunkSize + 2; // Sauter le chunk et "\r\n"
//     }
// }

// void RequestBody::parseContentLength(const std::string &rawData, size_t &index, size_t contentLength)
// {
//     // Vérifier s'il reste des données à lire
//     if (index >= rawData.size())
//         return;
    
//     // Calculer combien de données on peut lire
//     size_t remainingBytes = contentLength - _currentSize;
//     size_t availableBytes = rawData.size() - index;
//     size_t bytesToRead = (remainingBytes < availableBytes) ? remainingBytes : availableBytes;
    
//     // Ajouter au body
//     if (bytesToRead > 0)
//     {
//         _bodyStr.append(rawData.substr(index, bytesToRead));
//         _currentSize += bytesToRead;
//         index += bytesToRead;
//     }
    
//     // Vérifier si on a tout lu
//     if (_currentSize >= contentLength)
//     {
//         _isComplete = true;
//     }
// }
