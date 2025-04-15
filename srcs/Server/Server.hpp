#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include<sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sstream> 


#include "Config.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "BlocServer.hpp"
#include "LogManager.hpp"
#include "../includes/Webserv.hpp"
#include "Utils.hpp"
#include "Request.hpp"
#include "Response.hpp"

class   Client;

#define MAX_EVENTS 10

enum State
{
    CREATED,
    INITIALIZED,
    RUNNING,
    STOPPED
};

class Server
{
    friend class Client;
    friend class Request;

    private:
        int                         _state;
        int                         _socket;
        int                         _epoll_fd;


        Config                      _config;

        std::map<int, Client*>       _clients_map;
        std::map<int, Socket*>       _sockets_map;

    public:
        Server();
        ~Server();

        void                        init();
        void                        run();
        void                        stop();
        void                        close_client(int client_fd);
        void                        close_socket(int socket_fd);
        void                        close_all();
        void                        remove_from_epoll(int fd);

        void                        change_epoll_event(int socketFD, uint32_t EVENT);
        void                        log_epoll_fds();
        void                        log_clients_map() const;
        
        //run 
        void                        handleNewConnection(int socket_fd);
        void                        handleClientData(int client_fd);
        void                        handleGetRequest(int client_fd, const char *buffer);
        void                        handleEpollEvents();
        void                        checkAndStart();
        


        // Getters
        int                         get_state() const { return _state; }
        int                         get_socket() const { return _socket; }
        int                         get_epoll_fd() const { return _epoll_fd; }
        Config                      get_config() const { return _config; }
        const std::map<int, Client*>      &get_clients_map() const { return _clients_map; }
        const std::map<int, Socket*>      &get_sockets_map() const { return _sockets_map; }

        // Setters
        void                        set_state(int state) { _state = state; }
        void                        set_socket(int socket) { _socket = socket; }
        void                        set_epoll_fd(int epoll_fd) { _epoll_fd = epoll_fd; }
        void                        set_config(Config config) { _config = config; }
        void                        set_clients_map(std::map<int, Client*> clients_map) { _clients_map = clients_map; }
        void                        set_sockets_map(std::map<int, Socket*> sockets_map) { _sockets_map = sockets_map; }



};

#endif // SERVER_HPP