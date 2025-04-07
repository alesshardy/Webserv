#include "Server.hpp"


/**TEST TEMPORAIRE RESPONSE */
std::string extractFilePath(const std::string &request);
std::string getContentType(const std::string &filePath);
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
 * @brief initialise le serveur
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
 * 
 */
void Server::run()
{

    if (_state != INITIALIZED)
    {
        LogManager::log(LogManager::ERROR, "Server not initialized");
        throw std::runtime_error("Server not initialized");
    }

    LogManager::log(LogManager::INFO, "Server running");
    _state = RUNNING;

    //debug : afficher les sockets
    for (std::map<int, Socket *>::iterator it = _sockets_map.begin(); it != _sockets_map.end(); ++it)
    {
        LogManager::log(LogManager::DEBUG, "Socket: %d", it->first);
    }
    //debug : afficher les ports associes aux sockets
    for (std::map<int, Socket *>::iterator it = _sockets_map.begin(); it != _sockets_map.end(); ++it)
    {
        LogManager::log(LogManager::DEBUG, "Port: %d", it->second->get_port());
    }

    // Boucle principale pour gerer les evenements
    while (_state == RUNNING && running)
    {
        LogManager::log(LogManager::DEBUG, "value of running: %d", running);
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1); 

        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                LogManager::log(LogManager::INFO, "epoll_wait interrupted by signal");
                continue; // Reprendre la boucle
            }
            LogManager::log(LogManager::ERROR, "Error in epoll_wait");
            throw std::runtime_error("Error in epoll_wait");
        }

        for (int n = 0; n < nfds; ++n)
        {
            //gerer les evenements
            if (events[n].events & EPOLLIN)
            {
                // Nouvelle connexion entrante
                if (_sockets_map.find(events[n].data.fd) != _sockets_map.end()) // si 
                {
                    // Accepter la connexion
                    int client_fd = accept(events[n].data.fd, NULL, NULL);
                    if (client_fd == -1)
                    {
                        LogManager::log(LogManager::ERROR, "Error accepting connection");
                        throw std::runtime_error("Error accepting connection");
                    }

                    // Créer un nouveau client
                    //Socket *socket = _sockets_map[events[n].data.fd];
                    Client *client = new Client(client_fd, _sockets_map[_socket]);
                    _clients_map[client_fd] = client;

                    // Ajouter le client à epoll
                    struct epoll_event ev;
                    ev.events = EPOLLIN; // Surveiller les événements de lecture
                    ev.data.fd = client_fd;

                    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                    {
                        LogManager::log(LogManager::ERROR, "Failed to add client to epoll");
                        delete client;
                        _clients_map.erase(client_fd);
                        close(client_fd);
                        continue;
                        // throw std::runtime_error("Failed to add client to epoll");
                    }

                    LogManager::log(LogManager::INFO, "New client connected: %d", client_fd);
                }
                else
                {
                    // Lecture des données du client
                    int client_fd = events[n].data.fd;
                    Client *client = _clients_map[client_fd];

                    // Utilisation temporaire de la variable `client`
                    LogManager::log(LogManager::DEBUG, "Client object address: %p", static_cast<void*>(client));

                    LogManager::log(LogManager::DEBUG, "Handling client %d", client_fd);

                    // Lire les données du client
                    char buffer[1024];
                    int bytes = read(client_fd, buffer, sizeof(buffer));
                    

                    if (bytes == -1)
                    {
                        LogManager::log(LogManager::ERROR, "Error reading from client %d", client_fd);
                        throw std::runtime_error("Error reading from client");
                    }
                    else if (bytes == 0)
                    {
                        // Fin de fichier
                        LogManager::log(LogManager::INFO, "Client %d disconnected", client_fd);
                        close_client(client_fd);
                    }
                    else
                    {
                        // Traiter les données lues
                        buffer[bytes] = '\0';
                        LogManager::log(LogManager::INFO, "Received %d bytes from client %d: %s", bytes, client_fd, buffer);

                        /***********TEST de reponse en dur******************************************************/
                        /***************************************************************************************/

                        // Vérifier si c'est une requête GET
                        if (strncmp(buffer, "GET", 3) == 0)
                        {
                            // Extraire le chemin du fichier demandé
                            std::string filePath = extractFilePath(buffer);

                            // Ouvrir le fichier
                            std::ifstream file(filePath.c_str(), std::ios::binary); // Utilisation de .c_str()
                            if (file.is_open())
                            {
                                // Lire le contenu du fichier
                                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                                file.close();

                                // Déterminer le type de contenu
                                std::string contentType = getContentType(filePath);

                                // Construire la réponse HTTP
                                std::ostringstream oss;
                                oss << content.size();
                                std::string response = "HTTP/1.1 200 OK\r\n";
                                response += "Content-Type: " + contentType + "\r\n";
                                response += "Content-Length: " + oss.str() + "\r\n";
                                response += "\r\n";
                                response += content;

                                // Envoyer la réponse au client
                                send(client_fd, response.c_str(), response.size(), 0);
                                LogManager::log(LogManager::INFO, "Response sent to client %d for file %s", client_fd, filePath.c_str());
                            }
                            else
                            {
                                // Fichier non trouvé
                                std::string response = "HTTP/1.1 404 Not Found\r\n";
                                response += "Content-Type: text/html\r\n";
                                response += "Content-Length: 58\r\n";
                                response += "\r\n";
                                response += "<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>";

                                send(client_fd, response.c_str(), response.size(), 0);
                                LogManager::log(LogManager::ERROR, "File not found: %s", filePath.c_str());
                            }
                        }
                        /***************************************************************************************/
                        /***********TEST de reponse en dur******************************************************/
                        
                    }
                }
            }
            
        }
    }
    LogManager::log(LogManager::DEBUG, "Value of running: %d", running);
    
    
    

}

//Arreter le serveur
void Server::stop()
{
    LogManager::log(LogManager::INFO, "Server stopping");
    _state = STOPPED;
    close_all();
    LogManager::log(LogManager::INFO, "Server stopped");

}


void Server::close_all()
{
    LogManager::log(LogManager::DEBUG, "Closing all sockets and clients started");
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

void Server::close_client(int client_fd)
{
    remove_from_epoll(client_fd); // Retirer le client de epoll

    // Fermer le client
    if (_clients_map.find(client_fd) != _clients_map.end())
    {
        Client *client = _clients_map[client_fd];
        if (client != NULL)
        {
            LogManager::log(LogManager::INFO, "Closing client %d", client_fd);
            delete client; // Supprime le client
            _clients_map.erase(client_fd);
        }
    }
    else
    {
        LogManager::log(LogManager::WARNING, "Client %d not found in _clients_map", client_fd);
    }

    close(client_fd); // Fermer le descripteur de fichier
}

void Server::close_socket(int socket_fd)
{
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
}

void Server::remove_from_epoll(int fd)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        LogManager::log(LogManager::WARNING, "Failed to remove FD %d from epoll", fd);
    }
}





//***************TEST DE REPONSE EN DUR TMP */


std::string extractFilePath(const std::string &request)
{
    size_t start = request.find("GET ") + 4;
    size_t end = request.find(" ", start);
    if (start == std::string::npos || end == std::string::npos)
        return "";
    std::string path = request.substr(start, end - start);
    if (path == "/")
        path = "/index.html"; // Par défaut, servir index.html
    return "www/main" + path; // Préfixer avec le répertoire racine
}

bool endsWith(const std::string &str, const std::string &suffix)
{
    if (str.length() >= suffix.length())
    {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}


std::string getContentType(const std::string &filePath)
{
    if (endsWith(filePath, ".html"))
        return "text/html";
    if (endsWith(filePath, ".css"))
        return "text/css";
    if (endsWith(filePath, ".js"))
        return "application/javascript";
    if (endsWith(filePath, ".png"))
        return "image/png";
    if (endsWith(filePath, ".jpg") || endsWith(filePath, ".jpeg"))
        return "image/jpeg";
    if (endsWith(filePath, ".gif"))
        return "image/gif";
    if (endsWith(filePath, ".ico"))
        return "image/x-icon";
    return "application/octet-stream"; // Type par défaut
}