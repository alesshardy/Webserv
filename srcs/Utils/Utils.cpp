#include "Utils.hpp"

int ft_stoi(const std::string &input)
{
    long long ret;
    std::istringstream iss(input);
    
    iss >> ret;
    if (iss.fail())
    {
        throw std::invalid_argument("stoi");
    }
    char extra;
    if (iss >> extra)
    {
        throw std::invalid_argument("stoi: extra characters found");
    }
    // Vérifier les limites de l'entier
    if (ret < std::numeric_limits<int>::min() || ret > std::numeric_limits<int>::max())
    {
      throw std::out_of_range("stoi: value out of range");
    }
    
    return static_cast<int>(ret);
}

unsigned long long ft_stoll(const std::string &input)
{
    unsigned long long ret;

    std::istringstream iss(input);
    
    iss >> ret;
    if (iss.fail())
    {
        throw std::invalid_argument("stoLONlong");
    }
    
    return ret;
}

std::string toString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

bool    directoryExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}
bool    fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}


bool createDirectory(const std::string& path) {
    return mkdir(path.c_str(), 0755) == 0; // Crée un répertoire avec des permissions 0755
}

std::string sanitizeFileName(const std::string& fileName)
{
    std::string sanitized;
    for (std::string::const_iterator it = fileName.begin(); it != fileName.end(); ++it)
    {
        char c = *it;
        if (isalnum(c) || c == '.' || c == '_' || c == '-')
            sanitized += c;
    }
    return sanitized;
}

std::string extractFileNameFromMultipart(const std::string& rawData)
{
    size_t dispositionPos = rawData.find("Content-Disposition:");
    if (dispositionPos == std::string::npos)
        return "";
    size_t filenamePos = rawData.find("filename=", dispositionPos);
    if (filenamePos == std::string::npos)
        return "";

    size_t start = rawData.find('"', filenamePos);
    size_t end = rawData.find('"', start + 1);
    if (start == std::string::npos || end == std::string::npos)
        return "";

    return rawData.substr(start + 1, end - start - 1);
}

bool isFileTransfer(const std::string& method, const std::map<std::string, std::string>& headers, const std::string& body)
{
    // Vérifier si la méthode est POST
    if (method != "POST")
        return false;

    // Vérifier si l'en-tête Content-Type est présent
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Type");
    if (it == headers.end())
        return false;

    // Vérifier si le Content-Type correspond à un transfert de fichier
    const std::string& contentType = it->second;
    if (contentType.find("multipart/form-data") != std::string::npos)
    {
        // Vérifier si un fichier est inclus dans le body
        if (body.find("filename=") != std::string::npos)
            return true;
        else
            return false; // Pas de fichier détecté
    }

    if (contentType.find("application/octet-stream") != std::string::npos)
    {
        return true; // Les données binaires sont considérées comme un transfert de fichier
    }

    return false;
}


std::string getStatusCodeMessage(int statusCode)
{
    switch (statusCode)
    {
        //succes
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";

        //redirection
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";

        //erreur client
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 429: return "Too Many Requests";

        //erreur serveur
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        default: return "Unknown Status Code";
    }
}