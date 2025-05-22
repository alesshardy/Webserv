#include "Socket.hpp"

// Constructors and destructors
Socket::Socket()
{
    _socket_fd = -1;
    _port = -1;
    _ip = "";
}

Socket::Socket(int port, std::string ip)
{
    _port = port;
    _ip = ip;
    create_socket();
    bind_socket();
    listen_socket();
}

Socket::~Socket()
{
    close_socket();
}

/**************************************  SOCKET *******************************************/

/**
 * @brief Créer un socket
 * 
 */
void Socket::create_socket()
{
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd == -1)
        throw std::runtime_error("Socket creation failed");

    fcntl(_socket_fd, F_SETFL, O_NONBLOCK); // rendre le socket non bloquant
    int opt = 1;
    setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // réutiliser l'adresse
}

/**
 * @brief Lier le socket à une adresse IP et un port
 * 
 */
void Socket::bind_socket()
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = inet_addr(_ip.c_str());

    if (bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
    {
        close(_socket_fd);
        throw std::runtime_error("Bind failed");
    }
}

/**
 * @brief Mettre le socket en écoute
 * 
 * @param backlog 
 */
void Socket::listen_socket(int backlog)
{
    if (listen(_socket_fd, backlog) < 0)
        throw std::runtime_error("Listen failed");
}

/**
 * @brief  Fermer le socket
 * 
 */
void Socket::close_socket()
{
    if (_socket_fd != -1)
        close(_socket_fd);
}