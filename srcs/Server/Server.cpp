#include "Server.hpp"


/**TEST TEMPORAIRE RESPONSE */
// std::string extractFilePath(const std::string &request);
// std::string getContentType(const std::string &filePath);
/**************** */


//Constructeur
Server::Server()
{
    _state = CREATED;
    _socket = -1;
    _epoll_fd = -1;
}
//Destructeur
Server::~Server()
{
    // for (std::map<int, Socket *>::iterator it = _sockets_map.begin(); it != _sockets_map.end(); ++it)
    // {
    //     delete it->second;
    // }
    // if (_epoll_fd != -1)
    // {
    //     close(_epoll_fd);
    // }
    // for (std::map<int, Client *>::iterator it = _clients_map.begin(); it != _clients_map.end(); ++it)
    // {
    //     delete it->second;
    // }
    close_all();
}



/**
 * @brief Initialise le serveur :
 * Cette méthode configure le serveur en créant des sockets pour chaque port spécifié dans la configuration.
 * 
 */
void Server::init()
{
    // Création de l'instance epoll
    _epoll_fd = epoll_create1(O_CLOEXEC); // surveiller les événements sur les descripteurs de fichiers.
    if (_epoll_fd == -1)
    {
        LogManager::log(LogManager::ERROR, "Error creating epoll instance");
        throw std::runtime_error("Failed to create epoll instance");
    }

    // Récupération des configurations des serveurs
    std::vector<BlocServer> servers = _config.getServers();

    for (size_t i = 0; i < servers.size(); i++)
    {
        const BlocServer &server = servers[i];
        const std::vector<Listen> &listens = server.getListen();

        for (size_t j = 0; j < listens.size(); j++)
        {
            int port = listens[j].getPort();
            std::string ip = listens[j].getIp();
            try
            {
                // Créer un socket pour chaque port (utilisation de new)
                Socket *socket = new Socket(port, ip);
                LogManager::log(LogManager::INFO, "Socket created for port %d", port);

                // Ajouter le socket à la map des sockets
                _sockets_map[socket->get_socket_fd()] = socket;

                // Si c'est le premier socket, l'assigner à _socket
                if (_socket == -1)
                {
                    _socket = socket->get_socket_fd();
                }

                // Ajouter le socket à epoll
                struct epoll_event ev;
                ev.events = EPOLLIN; // Surveiller les événements de lecture
                ev.data.fd = socket->get_socket_fd();

                if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, socket->get_socket_fd(), &ev) == -1) 
                {
                    LogManager::log(LogManager::ERROR, "Failed to add socket to epoll");
                    throw std::runtime_error("Failed to add socket to epoll");
                }

                LogManager::log(LogManager::INFO, "Socket added to epoll for port %d", port);
            }
            catch (const std::exception &e)
            {
                LogManager::log(LogManager::ERROR, "Error initializing socket for port %d: %s", port, e.what());
            }
        }
    }

    LogManager::log(LogManager::INFO, "Server initialization complete");
    _state = INITIALIZED;
}

/**
 * @brief Lancer le serveur
 * Cette méthode démarre le serveur en vérifiant son état et en gérant les événements epoll.
 * Elle entre dans une boucle d'attente d'événements jusqu'à ce que le serveur soit arrêté.
 */
void Server::run()
{
    checkAndStart();

    while (_state == RUNNING && running)
    {
        // LogManager::log(LogManager::DEBUG, "value of running: %d", running);
        handleEpollEvents();
    }

    // LogManager::log(LogManager::DEBUG, "Value of running: %d", running);

}

/***********************************************Handle run */

/**
 * @brief vérifie l'état du serveur et le démarre
 * 
 */
void Server::checkAndStart()
{
    if (_state != INITIALIZED)
    {
        LogManager::log(LogManager::ERROR, "Server not initialized");
        throw std::runtime_error("Server not initialized");
    }

    LogManager::log(LogManager::INFO, "Server running");
    _state = RUNNING;

    // Debug : afficher les sockets et ports associés
    for (std::map<int, Socket *>::iterator it = _sockets_map.begin(); it != _sockets_map.end(); ++it)
    {
        LogManager::log(LogManager::DEBUG, "Socket: %d", it->first);
        LogManager::log(LogManager::DEBUG, "Port: %d", it->second->get_port());
    }
}

/**
 * @brief Gérer les événements epoll.
 * Cette méthode attend les événements sur les descripteurs de fichiers surveillés par epoll.
 * Lorsqu'un événement est détecté, elle appelle la méthode appropriée pour traiter la connexion ou les données du client.
 * 
 */
void Server::handleEpollEvents()
{
    struct epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

    if (nfds == -1)
    {
        if (errno == EINTR)
        {
            LogManager::log(LogManager::INFO, "epoll_wait interrupted by signal");
            return; // Reprendre la boucle
        }
        LogManager::log(LogManager::ERROR, "Error in epoll_wait");
        throw std::runtime_error("Error in epoll_wait");
    }

    for (int n = 0; n < nfds; ++n)
    {
        int fd = events[n].data.fd;

     // Vérifiez si le FD est valide dans _clients_map ou _sockets_map
     if (_clients_map.find(fd) == _clients_map.end() && _sockets_map.find(fd) == _sockets_map.end())
     {
         LogManager::log(LogManager::WARNING, "Event on invalid or closed FD %d, skipping", fd);
         continue;
     }

        if (events[n].events & EPOLLERR)
        {
            LogManager::log(LogManager::ERROR, "Error on socket %d", fd);
            close_socket(fd);
            continue;
        }
        if (events[n].events & EPOLLHUP)
        {
            LogManager::log(LogManager::WARNING, "Socket %d hung up", fd);
            close_socket(fd);
            continue;
        }
        if (events[n].events & EPOLLRDHUP)
        {
            LogManager::log(LogManager::WARNING, "Socket %d read hang up", fd);
            close_socket(fd);
            continue;
        }
        if (events[n].events & EPOLLIN)
        {
            if (_sockets_map.find(fd) != _sockets_map.end())
            {
                handleNewConnection(fd);
                LogManager::log(LogManager::DEBUG, "New connection on socket %d", fd);
               
            }
            else
                handleClientData(fd);

        }

        if (events[n].events & EPOLLOUT)
        {
            if (_clients_map.find(fd) != _clients_map.end())
            {
                Client *client = _clients_map[fd];
                if (client->getRequest() && client->getRequest()->getState() == END)
                {
                    LogManager::log(LogManager::DEBUG, "Request is complete, sending response for FD %d", fd);
                    client->handleResponse(_epoll_fd);
                }
                else
                {
                    LogManager::log(LogManager::DEBUG, "Request is not complete for FD %d, waiting for more data", fd);
                }
            }
            else
            {
                LogManager::log(LogManager::WARNING, "EPOLLOUT event on invalid or closed FD %d, skipping", fd);
            }
        }
    }
}


/**
 * @brief Gérer une nouvelle connexion.
 * Cette méthode est appelée lorsqu'un nouveau client se connecte au serveur.
 * Elle accepte la connexion, crée un objet Client et l'ajoute à la map des clients.
 * Elle ajoute également le client à epoll pour surveiller les événements de lecture.
 * @param socket_fd 
 */
void Server::handleNewConnection(int socket_fd)
{
    int client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd == -1)
    {
        LogManager::log(LogManager::ERROR, "Error accepting connection");
        throw std::runtime_error("Error accepting connection");
    }

    // Vérifier si le client existe déjà dans _clients_map
    if (_clients_map.find(client_fd) != _clients_map.end())
    {
        LogManager::log(LogManager::WARNING, "Client %d already exists, deleting old client", client_fd);
        delete _clients_map[client_fd]; // Supprimer l'ancien client
        _clients_map.erase(client_fd);  // Retirer l'entrée de la map
    }

    Client *client = new Client(client_fd, _sockets_map[socket_fd], this);
    _clients_map[client_fd] = client;

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    {
        LogManager::log(LogManager::ERROR, "Failed to add client to epoll");
        delete client;
        _clients_map.erase(client_fd);
        close(client_fd);
        return;
    }

    LogManager::log(LogManager::INFO, "New client connected: %d", client_fd);
}

/**
 * @brief Gérer les données du client.
 * Cette méthode est appelée lorsqu'il y a des données à lire sur un client.
 * Elle lit les données, les traite et envoie une réponse au client.
 * @param client_fd 
 */
void Server::handleClientData(int client_fd)
{
    char buffer[1024] = {0};
    int bytes = read(client_fd, buffer, sizeof(buffer));
    if (bytes == -1)
    {
        LogManager::log(LogManager::ERROR, "Error reading from client %d", client_fd);
        close_client(client_fd);
        return;
    }
    else if (bytes == 0)
    {
        LogManager::log(LogManager::INFO, "Client %d disconnected", client_fd);
        close_client(client_fd);
        return;
    }
    // buffer[bytes] = '\0';
    std::string str(buffer, bytes);
    // std::cout << str << std::endl;
    // exit(EXIT_SUCCESS);
    _clients_map[client_fd]->handleRequest(str);
    // LogManager::log(LogManager::INFO, "Received %d bytes from client %d: %s", bytes, client_fd, buffer);

    /*******Test en dur de réponse */
    // if (strncmp(buffer, "GET", 3) == 0)
    // {
    //     handleGetRequest(client_fd, buffer);
    // }

}

/**
 * @brief Arrêter le serveur
 * 
 */
void Server::stop()
{
    LogManager::log(LogManager::INFO, "Server stopping");
    _state = STOPPED;
    close_all();
    LogManager::log(LogManager::INFO, "Server stopped");

}

// /**
//  * @brief Gérer une requête GET
//  * 
//  * @param client_fd 
//  * @param buffer
//  */
// void Server::handleGetRequest(int client_fd, const char *buffer)
// {
//     std::string filePath = extractFilePath(buffer);

//     std::ifstream file(filePath.c_str(), std::ios::binary);
//     if (file.is_open())
//     {
//         std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//         file.close();

//         std::string contentType = getContentType(filePath);

//         std::ostringstream oss;
//         oss << content.size();
//         std::string response = "HTTP/1.1 200 OK\r\n";
//         response += "Content-Type: " + contentType + "\r\n";
//         response += "Content-Length: " + oss.str() + "\r\n";
//         response += "\r\n";
//         response += content;

//         send(client_fd, response.c_str(), response.size(), 0);
//         LogManager::log(LogManager::INFO, "Response sent to client %d for file %s", client_fd, filePath.c_str());
//     }
//     else
//     {
//         std::string response = "HTTP/1.1 404 Not Found\r\n";
//         response += "Content-Type: text/html\r\n";
//         response += "Content-Length: 58\r\n";
//         response += "\r\n";
//         response += "<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>";

//         send(client_fd, response.c_str(), response.size(), 0);
//         LogManager::log(LogManager::ERROR, "File not found: %s", filePath.c_str());
//     }
// }

/**
 * @brief Fermer tous les sockets et clients
 * 
 */
void Server::close_all()
{
    LogManager::log(LogManager::DEBUG, "Closing all sockets and clients started");


    log_clients_map();

    
    // Fermer les clients
    for (std::map<int, Client*>::iterator it = _clients_map.begin(); it != _clients_map.end();)
    {
        int client_fd = it->first;
        ++it; // Incrémenter l'itérateur avant de supprimer le client
        close_client(client_fd);
    }
    _clients_map.clear();

    LogManager::log(LogManager::DEBUG, "Clients closed");
    // Fermer les sockets
    for (std::map<int, Socket*>::iterator it = _sockets_map.begin(); it != _sockets_map.end();)
    {
        int socket_fd = it->first;
        ++it; // Incrémenter l'itérateur avant de supprimer le socket
        close_socket(socket_fd);
    }
    _sockets_map.clear();

    LogManager::log(LogManager::DEBUG, "Sockets closed");
    // Fermer l'instance epoll
    if (_epoll_fd != -1)
    {
        close(_epoll_fd);
        _epoll_fd = -1;
    }


    LogManager::log(LogManager::DEBUG, "Epoll instance closed");

    _state = CREATED;
}

/**
 * @brief Fermer un client
 * 
 * @param client_fd 
 */
void Server::close_client(int client_fd)
{
    LogManager::log(LogManager::DEBUG, "Closing client FD %d", client_fd);

    // Retirer le FD d'epoll
    remove_from_epoll(client_fd);

    // Supprimer le client de la map
    if (_clients_map.find(client_fd) != _clients_map.end())
    {
        Client *client = _clients_map[client_fd];
        if (client != NULL)
        {
            LogManager::log(LogManager::INFO, "Deleting client object for FD %d", client_fd);
            delete client;
            _clients_map.erase(client_fd);
        }
    }
    else
    {
        LogManager::log(LogManager::WARNING, "Client FD %d not found in _clients_map", client_fd);
    }

    // Fermer le descripteur de fichier
    if (close(client_fd) == -1)
    {
        LogManager::log(LogManager::WARNING, "Failed to close FD %d: %s", client_fd, strerror(errno));
    }
    else
    {
        LogManager::log(LogManager::DEBUG, "Successfully closed FD %d", client_fd);
    }
}

/**
 * @brief Fermer un socket
 * 
 * @param socket_fd 
 */
void Server::close_socket(int socket_fd)
{
    LogManager::log(LogManager::DEBUG, "Closing socket %d", socket_fd);
    remove_from_epoll(socket_fd); // Retirer le socket de epoll

    // Fermer le socket
    if (_sockets_map.find(socket_fd) != _sockets_map.end())
    {
        Socket *socket = _sockets_map[socket_fd];
        if (socket != NULL)
        {
            LogManager::log(LogManager::INFO, "Closing socket %d", socket_fd);
            delete socket; // Supprime le socket
            _sockets_map.erase(socket_fd);
        }
    }
    else
    {
        LogManager::log(LogManager::WARNING, "Socket %d not found in _sockets_map", socket_fd);
    }
    // Fermer le descripteur de fichier
    if (close(socket_fd) == -1)
    {
        LogManager::log(LogManager::WARNING, "Failed to close FD %d: %s", socket_fd, strerror(errno));
    }
}

void Server::remove_from_epoll(int fd)
{
    if (fd < 0)
    {
        LogManager::log(LogManager::WARNING, "Invalid FD %d, skipping removal from epoll", fd);
        return;
    }
    LogManager::log(LogManager::DEBUG, "Attempting to remove FD %d from epoll", fd);
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        LogManager::log(LogManager::WARNING, "Failed to remove FD %d from epoll: %s", fd, strerror(errno));
    }
    else
    {
        LogManager::log(LogManager::DEBUG, "Successfully removed FD %d from epoll", fd);
    }
}


void Server::change_epoll_event(int socketFD, uint32_t EVENT)
{
    int epollFD = _epoll_fd;
    struct epoll_event ev;
    ev.events = EVENT;
    ev.data.fd = socketFD;

    if (epoll_ctl(epollFD, EPOLL_CTL_MOD, socketFD, &ev) == -1)
    {
        LogManager::log(LogManager::ERROR, "Failed to modify epoll event for socket %d", socketFD);
        throw std::runtime_error("Failed to modify epoll event");
    }
}





//***************TEST DE REPONSE EN DUR TMP */


// std::string extractFilePath(const std::string &request)
// {
//     size_t start = request.find("GET ") + 4;
//     size_t end = request.find(" ", start);
//     if (start == std::string::npos || end == std::string::npos)
//         return "";
//     std::string path = request.substr(start, end - start);
//     if (path == "/")
//         path = "/index.html"; // Par défaut, servir index.html
//     return "www/main" + path; // Préfixer avec le répertoire racine
// }

// bool endsWith(const std::string &str, const std::string &suffix)
// {
//     if (str.length() >= suffix.length())
//     {
//         return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
//     }
//     return false;
// }


// std::string getContentType(const std::string &filePath)
// {
//     if (endsWith(filePath, ".html"))
//         return "text/html";
//     if (endsWith(filePath, ".css"))
//         return "text/css";
//     if (endsWith(filePath, ".js"))
//         return "application/javascript";
//     if (endsWith(filePath, ".png"))
//         return "image/png";
//     if (endsWith(filePath, ".jpg") || endsWith(filePath, ".jpeg"))
//         return "image/jpeg";
//     if (endsWith(filePath, ".gif"))
//         return "image/gif";
//     if (endsWith(filePath, ".ico"))
//         return "image/x-icon";
//     return "application/octet-stream"; // Type par défaut
// }


////debug EPOLL**************************************

void Server::log_epoll_fds()
{
    LogManager::log(LogManager::DEBUG, "Listing FDs currently in epoll:");
    struct epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, 0); // Timeout 0 to avoid blocking

    if (nfds == -1)
    {
        if (errno != EINTR)
        {
            LogManager::log(LogManager::ERROR, "Error listing epoll FDs: %s", strerror(errno));
        }
        return;
    }

    for (int i = 0; i < nfds; ++i)
    {
        LogManager::log(LogManager::DEBUG, "FD in epoll: %d", events[i].data.fd);
    }
}


void Server::log_clients_map() const
{
    if (_clients_map.empty())
    {
        LogManager::log(LogManager::DEBUG, "No clients in _clients_map.");
        return;
    }

    LogManager::log(LogManager::DEBUG, "Listing clients in _clients_map:");
    for (std::map<int, Client*>::const_iterator it = _clients_map.begin(); it != _clients_map.end(); ++it)
    {
        int client_fd = it->first;
        Client* client = it->second;
        if (client)
        {
            LogManager::log(LogManager::DEBUG, "Client FD: %d, Client Pointer: %p", client_fd, client);
        }
        else
        {
            LogManager::log(LogManager::WARNING, "Client FD: %d has a NULL pointer in _clients_map.", client_fd);
        }
    }
}