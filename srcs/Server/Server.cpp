#include "Server.hpp"


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
    for (std::map<int, Socket *>::iterator it = _sockets_map.begin(); it != _sockets_map.end(); ++it)
    {
        delete it->second;
    }
    if (_epoll_fd != -1)
    {
        close(_epoll_fd);
    }
    for (std::map<int, Client *>::iterator it = _clients_map.begin(); it != _clients_map.end(); ++it)
    {
        delete it->second;
    }
}



/**
 * @brief initialise le serveur
 * 
 */
void Server::init()
{
    // Création de l'instance epoll
    _epoll_fd = epoll_create1(O_CLOEXEC);
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
        const std::vector<int> &ports = server.getListen();

        for (size_t j = 0; j < ports.size(); j++)
        {
            int port = ports[j];
            try
            {
                // Créer un socket pour chaque port (utilisation de new)
                Socket *socket = new Socket(port, "0.0.0.0"); // Écoute sur toutes les interfaces
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

//Lancer le serveur

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
    while (_state == RUNNING)
    {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            LogManager::log(LogManager::ERROR, "Error in epoll_wait");
            throw std::runtime_error("Error in epoll_wait");
        }

        for (int n = 0; n < nfds; ++n)
        {
            //gerer les evenements
            if (events[n].events & EPOLLIN)
            {
                // Nouvelle connexion entrante
                if (_sockets_map.find(events[n].data.fd) != _sockets_map.end())
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
                    }
                }
            }
            
        }
    }
    
    
    

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
    // Fermer les clients
    for (std::map<int, Client*>::iterator it = _clients_map.begin(); it != _clients_map.end(); ++it)
    {
        close_client(it->first);
    }

    // Fermer les sockets
    for (std::map<int, Socket*>::iterator it = _sockets_map.begin(); it != _sockets_map.end(); ++it)
    {
        close_socket(it->first);
    }

    // Fermer l'instance epoll
    if (_epoll_fd != -1)
    {
        close(_epoll_fd);
        _epoll_fd = -1;
    }

    _state = CREATED;
}

void Server::close_client(int client_fd)
{
    // Fermer le client
    Client *client = _clients_map[client_fd];
    if (client != NULL)
    {
        LogManager::log(LogManager::INFO, "Closing client %d", client_fd);
        delete client;
        _clients_map.erase(client_fd);
    }

    // Fermer le socket du client
    close_socket(client_fd);
}

void Server::close_socket(int socket_fd)
{
    // Fermer le socket
    Socket *socket = _sockets_map[socket_fd];
    if (socket != NULL)
    {
        LogManager::log(LogManager::INFO, "Closing socket %d", socket_fd);
        delete socket;
        _sockets_map.erase(socket_fd);
    }
}