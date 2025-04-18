#include "RequestBody.hpp"


// Constructeur Et destructeur
RequestBody::RequestBody(size_t maxBodySize, bool isChunked)
{   
    _currentSize = 0;
    _maxBodySize = maxBodySize;
    _isChunked = isChunked;
    _isComplete = false;

    char tmpFileName[] = "/tmp/request_body_XXXXXX";
    _fd = mkstemp(tmpFileName); // fonction pour creer un fichier temporaire
    if (_fd == -1)
    {
        perror("mkstemp failed");
        throw std::runtime_error("ERROR: failed to create tmp file");
    }
    
    _tmpFilePath = tmpFileName;
}

RequestBody::~RequestBody()
{
    if (_fd != -1)
    close (_fd);
    if (!_tmpFilePath.empty())
    std::remove(_tmpFilePath.c_str());
}
/************************** VERSION AVEC FICHIER TEMP & FD *****************************/

/**
 * @brief  Parse le body de la requête en mode content length.
 * 
 * @param rawData 
 * @param index 
 * @param contentLength 
 */
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
        if (write(_fd, rawData.data() + index, bytesToRead) == -1)
            throw std::runtime_error("ERROR: failed to write in tmp file");
        
        _currentSize += bytesToRead;
        index += bytesToRead;
    }

    if (_currentSize == contentLength)
        _isComplete = true;
    else if (_currentSize > contentLength)
        throw std::runtime_error("ERROR: Body size exceeds Content-Length");
    
}

/**
 * @brief Parse le body de la requête en mode chunked.
 * 
 * @param rawData 
 * @param index 
 */
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
                if (index != rawData.size())
                    throw std::runtime_error("ERROR: Unexpected data after final chunk");
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
        if (write(_fd, rawData.data() + index, chunkSize) == -1)
            throw std::runtime_error("ERROR: failed to write in tmp file");

        _currentSize += chunkSize;

        // Vérifier que les deux caractères suivant le chunk sont "\r\n"
        if (rawData.substr(index + chunkSize, 2) != "\r\n")
            throw std::runtime_error("ERROR: Chunked size announce don't match reality");

        // Avancer au chunk suivant
        index += chunkSize + 2; // Sauter le chunk et "\r\n"]
    }
}

/************************************************* PRINT *******************************************************/

/**
 * @brief  Lit le contenu du fichier temporaire et le retourne sous forme de chaîne.
 * 
 * @return std::string 
 */
std::string RequestBody::readBody() const
{
    if (_fd == -1)
        throw std::runtime_error("ERROR: Temporary file not initialized");

    std::ifstream tmpFile(_tmpFilePath.c_str(), std::ios::binary);
    if (!tmpFile.is_open())
        throw std::runtime_error("ERROR: Failed to open temporary file");

    std::ostringstream oss;
    oss << tmpFile.rdbuf();
    return oss.str();
}