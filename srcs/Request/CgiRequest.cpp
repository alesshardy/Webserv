#include "CgiRequest.hpp"

CgiRequest::CgiRequest(Request *request, std::string cgiPath, std::string scriptCgi): _request(request), _cgiPath(cgiPath), _scriptCgi(scriptCgi)
{
    _initEnv();

    char tmpFileName[] = "/tmp/cgi_tmpfile_XXXXXX";
    _fd = mkstemp(tmpFileName); // fonction pour creer un fichier temporaire
    if (_fd == -1)
    {
        perror("mkstemp failed");
        throw std::runtime_error("ERROR: failed to create tmp file");
    }
    _tmpFilePath = tmpFileName;
}

CgiRequest::CgiRequest(CgiRequest const & copy)
{
    *this = copy;
}

CgiRequest::~CgiRequest()
{
    if (_envp)
    {
        for (size_t i = 0; _envp[i] != NULL; i++)
            free(_envp[i]);
        delete [] _envp;
    }
}

void CgiRequest::_initEnv()
{  
    LogManager::log(LogManager::DEBUG, ("Init ENV pour CGI"));

    // Initialiser les variables d'environnement CGI
    _env["SERVER_NAME"] = "Webserv";
    _env["SERVER_SOFTWARE"] = "Webserv/1.0";
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["REQUEST_METHOD"] = _request->getMethod();
    _env["SCRIPT_NAME"] = _request->getUri();
    _env["QUERY_STRING"] = _request->getQueryString().empty() ? "" : _request->getQueryString();
    _env["CONTENT_TYPE"] = _request->getHeaders().count("Content-Type") ? _request->getHeaders().at("Content-Type") : "";
    _env["CONTENT_LENGTH"] = _request->getHeaders().count("Content-Length") ? _request->getHeaders().at("Content-Length") : "";
    _env["HTTP_HOST"] = _request->getHeaders().count("Host") ? _request->getHeaders().at("Host") : "";
    _env["SERVER_PORT"] = toString(_request->getPort());
    _env["PATH_INFO"] = _request->getUri();
    _env["PATH_TRANSLATED"] = _request->getMatchingLocation()->getRoot() + _request->getUri();
}

void    CgiRequest::_CgiConvertEnvToChar()
{
    LogManager::log(LogManager::DEBUG, ("Convert Env to Char[]:"));
    _envp = new char*[_env.size() + 1];

    size_t i= 0;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        std::string envStr = it->first + "=" + it->second;
        _envp[i] = strdup(envStr.c_str());
        i++;
    }

    _envp[i] = NULL;
    if (LogManager::getDebugLogStatus())
    {
        // Debug : Afficher les variables d'environnement converties
        for (size_t j = 0; j < i; ++j)
            std::cout << _envp[j] << std::endl;
    }
}
void CgiRequest::executeCgi()
{
    if (LogManager::getDebugLogStatus())
        _printEnv();

    _CgiConvertEnvToChar();

    // Préparer les arguments pour execve
    _args[0] = const_cast<char*>(_cgiPath.c_str());
    _args[1] = const_cast<char*>(_scriptCgi.c_str());
    _args[2] = NULL;

    // Créer des pipes pour stdin et stdout
    int pipeIn[2];
    int pipeOut[2];
    if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
        throw std::runtime_error("Failed to create pipes");

    // Stocker les descripteurs dans les membres de la classe
    _stdin = pipeIn[1];  // Écriture dans le pipe d'entrée
    _stdout = pipeOut[0]; // Lecture depuis le pipe de sortie

    // Créer un processus enfant
    _pid = fork();
    if (_pid == -1)
    {
        throw std::runtime_error("Failed to fork process");
    }
    else if (_pid == 0) // Processus enfant
    {
        // Rediriger stdin et stdout
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);

        // Fermer les extrémités inutilisées des pipes
        close(pipeIn[0]);
        close(pipeIn[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);

        // Exécuter le script CGI
        execve(_cgiPath.c_str(), _args, _envp);

        // Si execve échoue
        exit(1);
    }
    else // Processus parent
    {
        // Fermer les extrémités inutilisées des pipes
        close(pipeIn[0]);
        close(pipeOut[1]);

        // Écrire dans le pipe d'entrée si nécessaire (par exemple, pour POST)
        if (_request->getMethod() == "POST")
        {
            const std::string &body = _request->getBody()->readBody();
            write(_stdin, body.c_str(), body.size());
        }
        close(_stdin); // Fermer l'écriture après avoir envoyé les données

        // Attendre la fin du processus CGI
        int status;
        waitpid(_pid, &status, 0); // SIUUUUU FLAg WUANG

        // Fermer la lecture depuis le pipe de sortie
        close(_stdout);
        _stdout = -1;
    }
}


// print env
void   CgiRequest::_printEnv()
{
    LogManager::log(LogManager::DEBUG, ("Print ENV:"));
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        std::cout << it->first << " = " << it->second << std::endl;
    }
}