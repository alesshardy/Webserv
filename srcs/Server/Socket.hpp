#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# include "LogManager.hpp"
// #include "BlocServer.hpp"
//class BlocServer;

class Socket
{
    private:
        int                     _socket_fd;
        int                     _port;
        std::string             _ip;
        struct sockaddr_in      _addr;
        //std::vector<BlocServer> _blocs;

    public:
        Socket();
        Socket(int port, std::string ip);
        ~Socket();

        int get_socket_fd() const { return _socket_fd; }
        int get_port() const { return _port; }
        std::string get_ip() const { return _ip; }
        struct sockaddr_in get_addr() const { return _addr; }

        void create_socket();
        void bind_socket();
        void listen_socket(int backlog = 10);
       // int accept_socket();
        void close_socket();
};

#endif // SOCKET_HPP