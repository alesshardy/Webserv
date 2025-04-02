#include "Listen.hpp"

Listen::Listen() : _ip("0.0.0.0"), _port(80)
{
}

Listen::Listen(const std::string &ip, int port) : _ip(ip), _port(port)
{
}

