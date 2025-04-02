#ifndef LISTEN_HPP
# define LISTEN_HPP

# include <string>
# include <sstream>

class Listen
{
    private:
            std::string _ip;
            int         _port;

    public:
            Listen();
            Listen(const std::string &ip, int port);

            //Getters
            const std::string &getIp() const {return _ip;};
            int getPort() const {return _port;};

            //setters
            void    setIp(const std::string &ip) {_ip = ip;};
            void    setPort(int port) {_port = port;};

            //Methods
};

#endif