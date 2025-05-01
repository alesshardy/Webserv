#include "Server.hpp"

//Constructeur
Server::Server()
{
    _state = CREATED;
    _socket = -1;
    _epoll_fd = -1;
    _config = Config();
}
//Destructeur
Server::~Server()
{
    close_all();
}
/***************************************************** SERVER  **************************************************************/

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
                LogManager::log(LogManager::DEBUG, "Socket created for port %d", port);

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

                LogManager::log(LogManager::DEBUG, "Socket added to epoll for port %d", port);
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
        handleEpollEvents();
    }
}


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
    int timeout = 1000; // timeout de 1sec pour relancer epoll_wait
    int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, timeout);

    // Vérifier le résultat de epoll_wait
    if (nfds < 0) // Erreur
    {
        if (running)
        {    
            LogManager::log(LogManager::ERROR, "epoll_wait failed");
            throw std::runtime_error("Error in epoll_wait");
        }
    }
    if (nfds == 0)
    {
        checkRequestTimeouts();
        return ;
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
        if (events[n].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        {
            LogManager::log(LogManager::ERROR, "ERROR : client disconnected ");
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
                if (client->getRequest() && client->getRequest()->getState() == CGI)
                {

                        client->getRequest()->getCgi()->checkEnd();
                    if (client->getRequest()->getState() == END)
                    {
                        LogManager::log(LogManager::DEBUG, "CGI finished, ready to send response for FD %d", fd);
                        client->getResponse()->setTimeStartResponse();
                        client->handleResponse(_epoll_fd);
                    }
                    else
                    {
                        if (!client->getRequest()->getCgi()->getLogFlag())
                        {
                            LogManager::log(LogManager::DEBUG, "CGI not finished yet for FD %d, waiting", fd);
                            client->getRequest()->getCgi()->setFlagLog();
                        }
                    }
                }
                else if (client->getRequest() && (client->getRequest()->getState() == END || client->getRequest()->getState() == ERROR))
                {
                    LogManager::log(LogManager::DEBUG, "Request is complete, sending response for FD %d", fd);
                    client->getResponse()->setTimeStartResponse();
                    client->handleResponse(_epoll_fd);
                }
                else
                {
                    LogManager::log(LogManager::DEBUG, "Request is not complete for FD %d, waiting for more data", fd);
                }
            }
            else
            {
                LogManager::log(LogManager::DEBUG, "EPOLLOUT event on invalid or closed FD %d, skipping", fd);
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
        LogManager::log(LogManager::DEBUG, "Client %d already exists, deleting old client", client_fd);
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

    LogManager::log(LogManager::DEBUG, "New client connected: %d", client_fd);
}

/**
 * @brief Gérer les données du client.
 * Cette méthode est appelée lorsqu'il y a des données à lire sur un client.
 * Elle lit les données, les traite et envoie une réponse au client.
 * @param client_fd 
 */
void Server::handleClientData(int client_fd)
{
    char buffer[8192] = {0};
    int bytes = read(client_fd, buffer, sizeof(buffer));
    if (bytes == -1)
    {
        LogManager::log(LogManager::ERROR, "Error reading from client %d", client_fd);
        close_client(client_fd);
        return;
    }
    else if (bytes == 0)
    {
        LogManager::log(LogManager::DEBUG, "Client %d disconnected", client_fd);
        close_client(client_fd);
        return;
    }
    std::string str(buffer, bytes);
    std::cout << str << std::endl;
    _clients_map[client_fd]->handleRequest(str);
    // LogManager::log(LogManager::INFO, "Received %d bytes from client %d: %s", bytes, client_fd, buffer);
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

/**
 * @brief Fermer tous les sockets et clients
 * 
 */
void Server::close_all()
{
    LogManager::log(LogManager::DEBUG, "Closing all sockets and clients started");

    if (LogManager::getDebugLogStatus())
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
            LogManager::log(LogManager::DEBUG, "Deleting client object for FD %d", client_fd);
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
        LogManager::log(LogManager::DEBUG, "Failed to close FD %d: %s", client_fd, strerror(errno));
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

    // Fermer le socket
    if (_sockets_map.find(socket_fd) != _sockets_map.end())
    {
        Socket *socket = _sockets_map[socket_fd];
        if (socket != NULL)
        {
            LogManager::log(LogManager::DEBUG, "Closing socket %d", socket_fd);
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
        LogManager::log(LogManager::DEBUG, "Failed to Close FD %d: %s", socket_fd, strerror(errno));
    }
}

/**
 * @brief Supprimer un descripteur de fichier de epoll
 * 
 * @param fd 
 */
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
        LogManager::log(LogManager::DEBUG, "Failed to remove FD %d from epoll: %s", fd, strerror(errno));
    }
    else
    {
        LogManager::log(LogManager::DEBUG, "Successfully removed FD %d from epoll", fd);
    }
}

/**
 * @brief Modifier l'événement epoll pour un socket
 * 
 * @param socketFD 
 * @param EVENT 
 */
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

/*************************** DEBUG ***************************/
/**
 * @brief Afficher les clients actuellement dans _clients_map
 * 
 */
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

/*************************************** UTILS **************************************************/

/**
 * @brief Récupérer le bloc serveur correspondant à la requête
 * 
 * @param request 
 * @return BlocServer* 
 */
// BlocServer* Server::getMatchingServer(const Request* request) const 
// {
//     if (!request) 
//         throw std::runtime_error("ERROR: Request object is not initialized");

//     // Récupérer les en-têtes de la requête
//     std::map<std::string, std::string> headers = request->getHeaders();
//     if (headers.find("Host") == headers.end()) 
//         throw std::runtime_error("ERROR: Missing Host Header");


//     std::cout << headers.find("Host")->second << std::endl;
//     // Extraire le nom d'hôte et le port depuis l'en-tête Host
//     std::string hostValue = headers["Host"];
//     size_t colonPos = hostValue.find(':');
//     std::string hostName = hostValue.substr(0, colonPos);
//     int port = (colonPos != std::string::npos) ? ft_stoi(hostValue.substr(colonPos + 1)) : 80;

//     // Trouver le bloc serveur correspondant
//     const std::vector<BlocServer>& servers = _config.getServers();
//     BlocServer* matchingServer = NULL;

//     for (size_t i = 0; i < servers.size(); ++i) 
//     {
//         const BlocServer& server = _config.getServer(i); // Utilisation d'une référence constante

//         // Vérifier si le port correspond
//         bool portMatches = false;
//         for (size_t j = 0; j < server.getListen().size(); ++j) 
//         {
//             if (server.getListen()[j].getPort() == port) 
//             {
//                 portMatches = true;
//                 break;
//             }
//         }

//         // Vérifier si le server_name correspond
//         bool serverNameMatches = false;
//         for (size_t j = 0; j < server.getServerName().size(); ++j) 
//         {
//             if (server.getServerName()[j] == hostName) 
//             {
//                 serverNameMatches = true;
//                 break;
//             }
//         }

//         // Si le port et le server_name correspondent, sélectionner ce bloc
//         if (portMatches && serverNameMatches) 
//             return const_cast<BlocServer*>(&server); // Suppression du const pour retourner un pointeur non-const

//         // Si seul le port correspond, garder ce bloc comme fallback
//         if (portMatches && !matchingServer) 
//             matchingServer = const_cast<BlocServer*>(&server); // Suppression du const pour fallback
//     }

//     // Si aucun bloc serveur ne correspond, lever une erreur
//     if (!matchingServer) 
//         throw std::runtime_error("ERROR: No matching server block found for the request");

//     return matchingServer;
// }

// VErsion mis a jour
BlocServer* Server::getMatchingServer(const Request* request) const 
{
    if (!request) 
        throw std::runtime_error("ERROR: Request object is not initialized");

    // Récupérer les en-têtes de la requête
    std::map<std::string, std::string> headers = request->getHeaders();
    if (headers.find("Host") == headers.end()) 
        throw std::runtime_error("ERROR: Missing Host Header");

    std::string hostValue = headers["Host"];
    size_t colonPos = hostValue.find(':');
    std::string hostName = hostValue.substr(0, colonPos);

    // Utiliser le port stocké dans la requête
    int requestPort = request->getPort();

    // // Si un port est spécifié dans l'en-tête Host, vérifier qu'il correspond au port stocké
    // if (colonPos != std::string::npos)
    // {
    //     int hostPort = ft_stoi(hostValue.substr(colonPos + 1));
    //     if (hostPort != requestPort)
    //     {
    //         LogManager::log(LogManager::ERROR, "Port mismatch: Host header port (%d) does not match request port (%d)", hostPort, requestPort);
    //         throw std::runtime_error("ERROR: Port in Host header does not match request port");
    //     }
    // }

    // Trouver le bloc serveur correspondant
    const std::vector<BlocServer>& servers = _config.getServers();
    BlocServer* matchingServer = NULL;

    for (size_t i = 0; i < servers.size(); ++i) 
    {
        const BlocServer& server = _config.getServer(i);

        // Vérifier si le server_name correspond
        bool serverNameMatches = false;
        for (size_t j = 0; j < server.getServerName().size(); ++j) 
        {
            if (server.getServerName()[j] == hostName) 
            {
                serverNameMatches = true;
                break;
            }
        }

        // Si le server_name correspond, vérifier si le port est dans les listen
        if (serverNameMatches)
        {
            for (size_t j = 0; j < server.getListen().size(); ++j) 
            {
                if (server.getListen()[j].getPort() == requestPort) 
                {
                    return const_cast<BlocServer*>(&server); // Retourner immédiatement si le server_name et le port correspondent
                }
            }
        }

        // Vérifier si le port correspond (fallback si aucun server_name ne correspond)
        bool portMatches = false;
        for (size_t j = 0; j < server.getListen().size(); ++j) 
        {
            if (server.getListen()[j].getPort() == requestPort) 
            {
                portMatches = true;
                break;
            }
        }

        // Si seul le port correspond, garder ce bloc comme fallback
        if (portMatches && !matchingServer) 
            matchingServer = const_cast<BlocServer*>(&server);
    }

    // Si aucun bloc serveur ne correspond, lever une erreur
    if (!matchingServer) 
        throw std::runtime_error("ERROR: No matching server block found for the request");

    return matchingServer;
}
void Server::checkRequestTimeouts()
{
    for (std::map<int, Client*>::iterator it = _clients_map.begin(); it != _clients_map.end();)
    {
        Client *client = it->second;
        Request *request = client->getRequest();
        Response *response = client->getResponse();

        if (request)
        {
            // Vérifier si la requête a dépassé le timeout
            if (request->isTimeoutExceeded() && request->getState() != END && request->getState() != ERROR)
            {
                if (request->getState() == CGI)
                {
                    // Tuer le processus CGI
                    CgiRequest *cgi = request->getCgi();
                    if (cgi)
                    {
                        pid_t cgiPid = cgi->getPid();
                        if (kill(cgiPid, SIGKILL) == 0)
                        {
                            LogManager::log(LogManager::ERROR, "Killed CGI process %d for client FD %d due to timeout", cgiPid, it->first);
                        }
                        else
                        {
                            LogManager::log(LogManager::ERROR, "Failed to kill CGI process %d for client FD %d", cgiPid, it->first);
                        }
                    }
                }

                LogManager::log(LogManager::ERROR, "Request timeout exceeded for client FD %d", it->first);
                int client_fd = it->first;
                ++it;
                close_client(client_fd); // Fermez la connexion pour ce client
                continue;
            }

            // Vérifier si la requête est dans un état d'erreur
            if (request->getState() == ERROR && request->_sentToResponse == false) 
            {
                LogManager::log(LogManager::ERROR, "Request in ERROR state for client FD %d", it->first);
                change_epoll_event(it->first, EPOLLOUT);
                request->_sentToResponse = true;
                // int client_fd = it->first;
                // ++it;
                // close_client(client_fd); // Fermez la connexion pour ce client
                // continue;
            }
        }
        //ajouter check de temps

        if (response)
        {
            //verifier si la reponse a depasse le timeout
            if (response->isTimeoutExceeded() && response->getResponseState() != R_END && response->getResponseState() > R_INIT)
            {
                LogManager::log(LogManager::ERROR, "Request timeout exceeded for client FD %d", it->first);
                int client_fd = it->first;
                ++it;
                close_client(client_fd); // Fermez la connexion pour ce client
                continue;
            }
        }
        
        ++it; // Passer au client suivant
    }
}
